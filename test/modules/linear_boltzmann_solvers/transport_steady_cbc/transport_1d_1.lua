-- 1D Transport test with Vacuum and Incident-isotropic BC.
-- SDM: PWLD
-- Test: Max-value=0.49903 and 7.18243e-4
num_procs = 3





--############################################### Check num_procs
if (check_num_procs==nil and number_of_processes ~= num_procs) then
  Log(LOG_0ERROR,"Incorrect amount of processors. " ..
    "Expected "..tostring(num_procs)..
    ". Pass check_num_procs=false to override if possible.")
  os.exit(false)
end

--############################################### Setup mesh
nodes={}
N=100
L=30.0
xmin = 0.0
dx = L/N
for i=1,(N+1) do
  k=i-1
  nodes[i] = xmin + k*dx
end

meshgen1 = mesh.OrthogonalMeshGenerator.Create({ node_sets = {nodes} })
mesh.MeshGenerator.Execute(meshgen1)

--############################################### Set Material IDs
VolumeMesherSetMatIDToAll(0)

--############################################### Add materials
materials = {}
materials[1] = PhysicsAddMaterial("Test Material");
materials[2] = PhysicsAddMaterial("Test Material2");

PhysicsMaterialAddProperty(materials[1],TRANSPORT_XSECTIONS)
PhysicsMaterialAddProperty(materials[2],TRANSPORT_XSECTIONS)

PhysicsMaterialAddProperty(materials[1],ISOTROPIC_MG_SOURCE)
PhysicsMaterialAddProperty(materials[2],ISOTROPIC_MG_SOURCE)


num_groups = 168
PhysicsMaterialSetProperty(materials[1],TRANSPORT_XSECTIONS,
  CHI_XSFILE,"xs_3_170.cxs")
PhysicsMaterialSetProperty(materials[2],TRANSPORT_XSECTIONS,
  CHI_XSFILE,"xs_3_170.cxs")

--PhysicsMaterialSetProperty(materials[1],TRANSPORT_XSECTIONS,SIMPLEXS0,num_groups,0.1)
--PhysicsMaterialSetProperty(materials[2],TRANSPORT_XSECTIONS,SIMPLEXS0,num_groups,0.1)

src={}
for g=1,num_groups do
  src[g] = 0.0
end
--src[1] = 1.0
PhysicsMaterialSetProperty(materials[1],ISOTROPIC_MG_SOURCE,FROM_ARRAY,src)
PhysicsMaterialSetProperty(materials[2],ISOTROPIC_MG_SOURCE,FROM_ARRAY,src)

--############################################### Setup Physics
pquad0 = CreateProductQuadrature(GAUSS_LEGENDRE,40)
lbs_block =
{
  num_groups = num_groups,
  groupsets =
  {
    {
      groups_from_to = {0, 62},
      angular_quadrature_handle = pquad0,
      angle_aggregation_num_subsets = 1,
      groupset_num_subsets = 8,
      inner_linear_method = "gmres",
      l_abs_tol = 1.0e-6,
      l_max_its = 300,
      gmres_restart_interval = 100,
    },
    {
      groups_from_to = {63, num_groups-1},
      angular_quadrature_handle = pquad0,
      angle_aggregation_num_subsets = 1,
      groupset_num_subsets = 8,
      inner_linear_method = "gmres",
      l_abs_tol = 1.0e-6,
      l_max_its = 300,
      gmres_restart_interval = 100,
    },
  },
  sweep_type = "CBC"
}

bsrc={}
for g=1,num_groups do
  bsrc[g] = 0.0
end
bsrc[1] = 1.0/2

lbs_options =
{
  boundary_conditions =
  {
    {
      name = "zmin",
      type = "incident_isotropic",
      group_strength = bsrc
    }
  },
  scattering_order = 5,
  save_angular_flux = true
}

phys1 = lbs.DiscreteOrdinatesSolver.Create(lbs_block)
lbs.SetOptions(phys1, lbs_options)

--############################################### Initialize and Execute Solver
ss_solver = lbs.SteadyStateSolver.Create({lbs_solver_handle = phys1})

SolverInitialize(ss_solver)
SolverExecute(ss_solver)

--############################################### Get field functions
fflist,count = LBSGetScalarFieldFunctionList(phys1)

--############################################### Line plot
--Testing consolidated interpolation
cline = FFInterpolationCreate(LINE)
FFInterpolationSetProperty(cline,LINE_FIRSTPOINT,0.0,0.0,0.0001+xmin)
FFInterpolationSetProperty(cline,LINE_SECONDPOINT,0.0,0.0, 29.999+xmin)
FFInterpolationSetProperty(cline,LINE_NUMBEROFPOINTS, 50)

for k=165,165 do
  FFInterpolationSetProperty(cline,ADD_FIELDFUNCTION,fflist[k])
end

FFInterpolationInitialize(cline)
FFInterpolationExecute(cline)

--############################################### Volume integrations
vol0 = mesh.RPPLogicalVolume.Create({infx=true, infy=true, infz=true})
ffi1 = FFInterpolationCreate(VOLUME)
curffi = ffi1
FFInterpolationSetProperty(curffi,OPERATION,OP_MAX)
FFInterpolationSetProperty(curffi,LOGICAL_VOLUME,vol0)
FFInterpolationSetProperty(curffi,ADD_FIELDFUNCTION,fflist[1])

FFInterpolationInitialize(curffi)
FFInterpolationExecute(curffi)
maxval = FFInterpolationGetValue(curffi)

Log(LOG_0,string.format("Max-value1=%.5f", maxval))

ffi2 = FFInterpolationCreate(VOLUME)
curffi = ffi2
FFInterpolationSetProperty(curffi,OPERATION,OP_MAX)
FFInterpolationSetProperty(curffi,LOGICAL_VOLUME,vol0)
FFInterpolationSetProperty(curffi,ADD_FIELDFUNCTION,fflist[160])

FFInterpolationInitialize(curffi)
FFInterpolationExecute(curffi)
maxval = FFInterpolationGetValue(curffi)

Log(LOG_0,string.format("Max-value2=%.5e", maxval))

--############################################### Exports
if (master_export == nil) then
  FFInterpolationExportPython(cline)
end

--############################################### Plots
if (location_id == 0 and master_export == nil) then
  local handle = io.popen("python3 ZLFFI00.py")
end
