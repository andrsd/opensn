--############################################### Setup mesh
nodes={}
N=32
ds=2.0/N
for i=0,N do
    nodes[i+1] = -1.0 + i*ds
end
meshgen1 = mesh.OrthogonalMeshGenerator.Create({ node_sets = {nodes,nodes,nodes} })
mesh.MeshGenerator.Execute(meshgen1)

material = PhysicsAddMaterial("Test Material");

-- Set Material IDs
vol0 = chiLogicalVolumeCreate(RPP,-1000,1000,-1000,1000,-1000,1000)
VolumeMesherSetProperty(MATID_FROMLOGICAL,vol0,material)

VolumeMesherSetupOrthogonalBoundaries()

MeshHandlerExportMeshToVTK("Mesh")
--############################################### Add material properties


-- Set material properties
PhysicsMaterialAddProperty(material,SCALAR_VALUE,"k")
chiPhysicsMaterialSetProperty(material,"k",SINGLE_VALUE,1.0)

PhysicsMaterialAddProperty(material,SCALAR_VALUE,"q")
chiPhysicsMaterialSetProperty(material,"q",SINGLE_VALUE,1.0)


--############################################### Setup Physics
phys1 = chiDiffusionCreateSolver()
chiSolverSetBasicOption(phys1,"discretization_method","PWLC");
chiSolverSetBasicOption(phys1,"residual_tolerance",1.0e-6)
chiDiffusionSetProperty(phys1,"boundary_type",4,"reflecting")
chiDiffusionSetProperty(phys1,"boundary_type",5,"reflecting")

--############################################### Initialize and
--                                                Execute Solver
chiDiffusionInitialize(phys1)
chiDiffusionExecute(phys1)

----############################################### Visualize the field function
fflist,count = chiGetFieldFunctionList(phys1)
chiExportFieldFunctionToVTK(fflist[1],"Tutorial1Output","Temperature")
