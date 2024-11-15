-- 3D Transport test with Vacuum and Incident-isotropic BC.
-- SDM: PWLD
-- Test: Max-value=5.28310e-01 and 8.04576e-04

nodes = {}
N = 10
L = 5.0
xmin = -L / 2
dx = L / N
for i = 1, (N + 1) do
  k = i - 1
  nodes[i] = xmin + k * dx
end
znodes = {}
for i = 1, (N / 2 + 1) do
  k = i - 1
  znodes[i] = xmin + k * dx
end

-- meshgen1 = mesh.OrthogonalMeshGenerator.Create({
--   node_sets = {
--     { 0., 0.2, 0.4 }, { 0., 0.3, 0.6 }, { 0., 0.8 }
--   }
-- })
meshgen1 = mesh.FromFileMeshGenerator.Create({
  filename = "cube-tets.e"
})
mesh.MeshGenerator.Execute(meshgen1)

--############################################### Set Material IDs
vol0 = logvol.RPPLogicalVolume.Create({ infx = true, infy = true, infz = true })
mesh.SetMaterialIDFromLogicalVolume(vol0, 0)

--############################################### Add materials
materials = {}
materials[1] = mat.AddMaterial("Test Material")

num_groups = 1
-- mat.SetProperty(materials[1], TRANSPORT_XSECTIONS, OPENSN_XSFILE, "xs_graphite_pure.xs")
mat.SetProperty(materials[1], TRANSPORT_XSECTIONS, SIMPLE_ONE_GROUP, 0., .9)

mat.SetProperty(materials[1], ISOTROPIC_MG_SOURCE, FROM_ARRAY, { 1. })

--############################################### Setup Physics
pquad0 = aquad.CreateProductQuadrature(GAUSS_LEGENDRE_CHEBYSHEV, 1, 1)

lbs_block = {
  num_groups = num_groups,
  groupsets = {
    {
      groups_from_to = { 0, num_groups - 1 },
      angular_quadrature_handle = pquad0,
      angle_aggregation_type = "single",
      angle_aggregation_num_subsets = 1,
      groupset_num_subsets = 1,
      inner_linear_method = "petsc_gmres",
      l_abs_tol = 1.0e-6,
      l_max_its = 300,
      gmres_restart_interval = 100,
    },
  },
  sweep_type = "CBC",
}
-- bsrc = {}
-- for g = 1, num_groups do
--   bsrc[g] = 0.0
-- end
-- bsrc[1] = 1.0 / 4.0 / math.pi
lbs_options = {
  -- boundary_conditions = {
  --   { name = "xmin", type = "isotropic", group_strength = bsrc },
  -- },
  scattering_order = 0,
  save_angular_flux = true,
}
-- if reflecting then
--   table.insert(lbs_options.boundary_conditions, { name = "zmin", type = "reflecting" })
-- end

phys1 = lbs.DiscreteOrdinatesSolver.Create(lbs_block)
lbs.SetOptions(phys1, lbs_options)

--############################################### Initialize and Execute Solver
ss_solver = lbs.SteadyStateSolver.Create({ lbs_solver_handle = phys1 })

solver.Initialize(ss_solver)
solver.Execute(ss_solver)
