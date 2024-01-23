--############################################### Setup mesh
meshgen = chi_mesh.ExtruderMeshGenerator.Create
({
  inputs =
  {
    chi_mesh.FromFileMeshGenerator.Create
    ({
        filename="./tri_2mat_bc_1542.obj"
    }),
  },
  layers = {{z=1.1, n=2}, {z=2.1, n=3}},
  partitioner = chi.KBAGraphPartitioner.Create
  ({
    nx = 2, ny=2, nz=2,
    xcuts = {0.5}, ycuts = {0.5}, zcuts = {1.1},
  })
})

chi_mesh.MeshGenerator.Execute(meshgen)
chiMeshHandlerExportMeshToVTK("mesh_out")

--############################################### rest of the simulation
--############################################### Add materials
materials = {}
materials[1] = chiPhysicsAddMaterial("material-1");
materials[2] = chiPhysicsAddMaterial("material-2");

chiPhysicsMaterialAddProperty(materials[1],TRANSPORT_XSECTIONS)
chiPhysicsMaterialSetProperty(materials[1],TRANSPORT_XSECTIONS,
  CHI_XSFILE,"xs_1g_MatA.cxs")
chiPhysicsMaterialAddProperty(materials[2],TRANSPORT_XSECTIONS)
chiPhysicsMaterialSetProperty(materials[2],TRANSPORT_XSECTIONS,
  CHI_XSFILE,"xs_1g_MatB.cxs")

chiPhysicsMaterialAddProperty(materials[1],ISOTROPIC_MG_SOURCE)
chiPhysicsMaterialAddProperty(materials[2],ISOTROPIC_MG_SOURCE)
num_groups = 1
src1={}
src2={}
for g=1,num_groups do
  src1[g] = 0.0
  src2[g] = 300.0
end
chiPhysicsMaterialSetProperty(materials[1],ISOTROPIC_MG_SOURCE,FROM_ARRAY,src1)
chiPhysicsMaterialSetProperty(materials[2],ISOTROPIC_MG_SOURCE,FROM_ARRAY,src2)

--############################################### Setup the Angular Quadrature
nazimu = 4
npolar = 2
pquad = chiCreateProductQuadrature(GAUSS_LEGENDRE_CHEBYSHEV,nazimu,npolar)

--############################################### Setup LBS parameters
lbs_block =
{
  num_groups = num_groups,
  groupsets =
  {
    {
      groups_from_to = {0, 0},
      angular_quadrature_handle = pquad,
      angle_aggregation_type = "single",
      angle_aggregation_num_subsets = 1,
      inner_linear_method = "gmres",
      l_abs_tol = 1.0e-6,
      l_max_its = 300,
      gmres_restart_interval = 30,
    }
  }
}

lbs_options =
{
  scattering_order = 0,
}

phys = lbs.DiscreteOrdinatesSolver.Create(lbs_block)
lbs.SetOptions(phys, lbs_options)

--############################################### Initialize and Execute Solver
ss_solver = lbs.SteadyStateSolver.Create({lbs_solver_handle = phys})

chiSolverInitialize(ss_solver)
chiSolverExecute(ss_solver)

--############################################### Get field functions
fflist,count = chiLBSGetScalarFieldFunctionList(phys)
vtk_basename = "solution_out"
chiExportFieldFunctionToVTK(fflist[1],vtk_basename)

--############################################### Volume integrations
Det = chiLogicalVolumeCreate(RPP,0.2,0.4,0.2,0.4,-1.,1.)
curffi = chiFFInterpolationCreate(VOLUME)
chiFFInterpolationSetProperty(curffi,OPERATION,OP_MAX)
chiFFInterpolationSetProperty(curffi,LOGICAL_VOLUME,Det)
chiFFInterpolationSetProperty(curffi,ADD_FIELDFUNCTION,fflist[1])

chiFFInterpolationInitialize(curffi)
chiFFInterpolationExecute(curffi)
maxval = chiFFInterpolationGetValue(curffi)

chiLog(LOG_0,string.format("Max-value1=%.5e", maxval))
