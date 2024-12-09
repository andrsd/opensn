-- 2D Transport test. Vacuum and Isotropic BC. Balance.
num_procs = 4

-- Check num_procs
-- if (check_num_procs==nil and number_of_processes ~= num_procs) then
--     log.Log(LOG_0ERROR,"Incorrect amount of processors. " ..
--                       "Expected "..tostring(num_procs)..
--                       ". Pass check_num_procs=false to override if possible.")
--     os.exit(false)
-- end

-- Setup mesh
nodes = {}
N = 20
L = 5.0
xmin = -L / 2
dx = L / N
for i = 1, (N + 1) do
  k = i - 1
  nodes[i] = xmin + k * dx
end

meshgen1 = mesh.OrthogonalMeshGenerator.Create({ node_sets = { nodes, nodes } })
meshgen1:Execute()

-- Set Material IDs
vol0 = logvol.RPPLogicalVolume.Create({ infx = true, infy = true, infz = true })
mesh.SetMaterialIDFromLogicalVolume(vol0, 0, true)
vol1 = logvol.RPPLogicalVolume.Create({ xmin = -1000.0, xmax = 0.0, infy = true, infz = true })
mesh.SetMaterialIDFromLogicalVolume(vol1, 1, true)

-- Add materials
materials = {}
materials[1] = mat.AddMaterial("Test Material")
materials[2] = mat.AddMaterial("Test Material2")

num_groups = 168
xs_3_170 = xs.LoadFromOpenSn("xs_3_170.xs")
materials[1]:SetTransportXSections(xs_3_170)
materials[2]:SetTransportXSections(xs_3_170)

src = {}
for g = 1, num_groups do
  src[g] = 0.0
end
mg_src0 = xs.IsotropicMultiGroupSource.FromArray(src)
materials[1]:SetIsotropicMGSource(mg_src0)
src[1] = 1.0
mg_src1 = xs.IsotropicMultiGroupSource.FromArray(src)
materials[2]:SetIsotropicMGSource(mg_src1)

-- Setup Physics
fac = 1
pquad = aquad.CreateProductQuadrature(GAUSS_LEGENDRE_CHEBYSHEV, 4 * fac, 3 * fac)
aquad.OptimizeForPolarSymmetry(pquad, 4.0 * math.pi)

lbs_block = {
  num_groups = num_groups,
  groupsets = {
    {
      groups_from_to = { 0, 62 },
      angular_quadrature = pquad,
      angle_aggregation_num_subsets = 1,
      groupset_num_subsets = 1,
      inner_linear_method = "petsc_gmres",
      l_abs_tol = 1.0e-4,
      l_max_its = 300,
      gmres_restart_interval = 100,
    },
    {
      groups_from_to = { 63, num_groups - 1 },
      angular_quadrature = pquad,
      angle_aggregation_num_subsets = 1,
      groupset_num_subsets = 1,
      inner_linear_method = "petsc_gmres",
      l_abs_tol = 1.0e-4,
      l_max_its = 300,
      gmres_restart_interval = 100,
    },
  },
}
bsrc = {}
for g = 1, num_groups do
  bsrc[g] = 0.0
end
bsrc[1] = 1.0 / 4.0 / math.pi

lbs_options = {
  boundary_conditions = {
    {
      name = "xmin",
      type = "isotropic",
      group_strength = bsrc,
    },
  },
  scattering_order = 0,
  verbose_ags_iterations = true,
  max_ags_iterations = 100,
  ags_tolerance = 1.0e-6,
}

phys1 = lbs.DiscreteOrdinatesSolver.Create(lbs_block)
phys1:SetOptions(lbs_options)

-- Initialize and Execute Solver
ss_solver = lbs.SteadyStateSolver.Create({ lbs_solver = phys1 })

ss_solver:Initialize()
ss_solver:Execute()

phys1:ComputeBalance()

-- Get field functions
fflist = lbs.GetScalarFieldFunctionList(phys1)

-- Volume integrations
ffi1 = fieldfunc.FieldFunctionInterpolationVolume.Create()
curffi = ffi1
curffi:SetOperationType(OP_MAX)
curffi:SetLogicalVolume(vol0)
curffi:AddFieldFunction(fflist[1])

curffi:Initialize(curffi)
curffi:Execute()
maxval = curffi:GetValue()

log.Log(LOG_0, string.format("Max-value1=%.5f", maxval))

-- Volume integrations
ffi1 = fieldfunc.FieldFunctionInterpolationVolume.Create()
curffi = ffi1
curffi:SetOperationType(OP_MAX)
curffi:SetLogicalVolume(vol0)
curffi:AddFieldFunction(fflist[160])

curffi:Initialize()
curffi:Execute()
maxval = curffi:GetValue()

log.Log(LOG_0, string.format("Max-value2=%.5e", maxval))

-- Exports
if master_export == nil then
  fieldfunc.ExportToVTK(fflist[1], "ZPhi3D", "Phi")
end
