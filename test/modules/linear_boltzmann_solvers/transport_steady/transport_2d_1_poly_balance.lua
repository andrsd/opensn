-- 2D Transport test with Vacuum and Incident-isotropic BC.
-- SDM: PWLD
-- Test: Max-value=0.50758 and 2.52527e-04
num_procs = 4





--############################################### Check num_procs
-- if (check_num_procs==nil and number_of_processes ~= num_procs) then
--     log.Log(LOG_0ERROR,"Incorrect amount of processors. " ..
--                       "Expected "..tostring(num_procs)..
--                       ". Pass check_num_procs=false to override if possible.")
--     os.exit(false)
-- end

--############################################### Setup mesh
nodes={}
N=20
L=5
xmin = -L/2
dx = L/N
for i=1,(N+1) do
  k=i-1
  nodes[i] = xmin + k*dx
end

meshgen1 = mesh.OrthogonalMeshGenerator.Create({ node_sets = {nodes,nodes} })
mesh.MeshGenerator.Execute(meshgen1)

--############################################### Set Material IDs
vol0 = logvol.RPPLogicalVolume.Create({infx=true, infy=true, infz=true})
mesh.SetMaterialIDFromLogicalVolume(vol0,0)
vol1 = logvol.RPPLogicalVolume.Create({xmin=-1000.0, xmax=0.0, infy=true, infz=true})
mesh.SetMaterialIDFromLogicalVolume(vol1,1)


--############################################### Add materials
materials = {}
materials[1] = mat.AddMaterial("Test Material");
materials[2] = mat.AddMaterial("Test Material2");

mat.AddProperty(materials[1], TRANSPORT_XSECTIONS)
mat.AddProperty(materials[2], TRANSPORT_XSECTIONS)

mat.AddProperty(materials[1], ISOTROPIC_MG_SOURCE)
mat.AddProperty(materials[2], ISOTROPIC_MG_SOURCE)

num_groups = 1
mat.SetProperty(materials[1], TRANSPORT_XSECTIONS, OPENSN_XSFILE, "tests/transport_steady/simple_scatter.xs")
mat.SetProperty(materials[2], TRANSPORT_XSECTIONS, OPENSN_XSFILE, "tests/transport_steady/simple_scatter.xs")

src={}
for g=1,num_groups do
  src[g] = 0.0
end
mat.SetProperty(materials[1], ISOTROPIC_MG_SOURCE, FROM_ARRAY, src)
src[1] = 1.0
mat.SetProperty(materials[2], ISOTROPIC_MG_SOURCE, FROM_ARRAY, src)

--############################################### Setup Physics
fac=1
pquad = aquad.CreateProductQuadrature(GAUSS_LEGENDRE_CHEBYSHEV, 4*fac, 3*fac)
aquad.OptimizeForPolarSymmetry(pquad, 4.0*math.pi)

lbs_block =
{
  num_groups = num_groups,
  groupsets =
  {
    {
      groups_from_to = {0, 0},
      angular_quadrature_handle = pquad0,
      angle_aggregation_num_subsets = 1,
      groupset_num_subsets = 1,
      inner_linear_method = "gmres",
      l_abs_tol = 1.0e-8,
      l_max_its = 300,
      gmres_restart_interval = 100,
    },
  }
}
bsrc={}
for g=1,num_groups do
  bsrc[g] = 0.0
end
bsrc[1] = 1.0/4.0/math.pi

lbs_options =
{
  scattering_order = 0,
}

phys1 = lbs.DiscreteOrdinatesSolver.Create(lbs_block)
lbs.SetOptions(phys1, lbs_options)

ss_solver = lbs.SteadyStateSolver.Create({lbs_solver_handle = phys1})

solver.Initialize(ss_solver)
solver.Execute(ss_solver)

lbs.ComputeBalance(phys1)

--############################################### Get field functions
fflist,count = lbs.GetScalarFieldFunctionList(phys1)

--############################################### Slice plot
slice2 = fieldfunc.FFInterpolationCreate(SLICE)
fieldfunc.SetProperty(slice2,SLICE_POINT,{x = 0.0, y = 0.0, z = 0.025})
fieldfunc.SetProperty(slice2,ADD_FIELDFUNCTION,fflist[1])

fieldfunc.Initialize(slice2)
fieldfunc.Execute(slice2)

--############################################### Volume integrations
ffi1 = fieldfunc.FFInterpolationCreate(VOLUME)
curffi = ffi1
fieldfunc.SetProperty(curffi,OPERATION,OP_MAX)
fieldfunc.SetProperty(curffi,LOGICAL_VOLUME,vol0)
fieldfunc.SetProperty(curffi,ADD_FIELDFUNCTION,fflist[1])

fieldfunc.Initialize(curffi)
fieldfunc.Execute(curffi)
maxval = fieldfunc.GetValue(curffi)

log.Log(LOG_0,string.format("Max-value1=%.5f", maxval))

--############################################### Volume integrations
ffi1 = fieldfunc.FFInterpolationCreate(VOLUME)
curffi = ffi1
fieldfunc.SetProperty(curffi,OPERATION,OP_MAX)
fieldfunc.SetProperty(curffi,LOGICAL_VOLUME,vol0)
fieldfunc.SetProperty(curffi,ADD_FIELDFUNCTION,fflist[160])

fieldfunc.Initialize(curffi)
fieldfunc.Execute(curffi)
maxval = fieldfunc.GetValue(curffi)

log.Log(LOG_0,string.format("Max-value2=%.5e", maxval))

--############################################### Exports
if master_export == nil then
  fieldfunc.ExportPython(slice2)
  ExportFieldFunctionToVTKG(fflist[1],"ZPhi3D","Phi")
end

--############################################### Plots
if (location_id == 0 and master_export == nil) then
  local handle = io.popen("python ZPFFI00.py")
end
