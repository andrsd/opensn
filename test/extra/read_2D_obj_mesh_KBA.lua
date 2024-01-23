--############################################### Setup mesh
meshgen = chi_mesh.MeshGenerator.Create
({
  inputs =
  {
    chi_mesh.FromFileMeshGenerator.Create
    ({
        filename="./tri_2mat_bc_1542.obj"
    }),
  },
  partitioner = chi.KBAGraphPartitioner.Create
  ({
    nx = 2, ny=2, nz=1,
    xcuts = {0.5}, ycuts = {0.5},
  })
})
chi_mesh.MeshGenerator.Execute(meshgen)
chiMeshHandlerExportMeshToVTK("mesh_out")

--############################################### rest of the simulation
dofile("2Dtransport_simulation_part.lua")
