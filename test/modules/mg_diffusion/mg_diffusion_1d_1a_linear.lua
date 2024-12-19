-- Setup mesh
nodes = {}
N = 4
L = 1
xmin = -L / 2
dx = L / N
for i = 1, (N + 1) do
  k = i - 1
  nodes[i] = xmin + k * dx
end

meshgen1 = mesh.OrthogonalMeshGenerator.Create({ node_sets = { nodes } })
mesh.MeshGenerator.Execute(meshgen1)

-- Set Material IDs
mesh.SetUniformMaterialID(0)

-- Setboundary IDs
-- xmin,xmax,ymin,ymax,zmin,zmax
e_vol = logvol.RPPLogicalVolume.Create({ zmin = 0.49, xmax = 10.0, infz = true, infy = true })
w_vol = logvol.RPPLogicalVolume.Create({ zmin = -10.0, xmax = -0.49, infz = true, infy = true })

e_bndry = "0"
w_bndry = "1"

mesh.SetBoundaryIDFromLogicalVolume(e_vol, e_bndry)
mesh.SetBoundaryIDFromLogicalVolume(w_vol, w_bndry)

-- materials
materials = {}
materials[1] = mat.AddMaterial("Test Material")

num_groups = 1
mat.SetProperty(materials[1], TRANSPORT_XSECTIONS, SIMPLE_ONE_GROUP, 1., 0.9)

src = {}
for g = 1, num_groups do
  src[g] = 0.0
end
src[1] = 1.0
mat.SetProperty(materials[1], ISOTROPIC_MG_SOURCE, FROM_ARRAY, src)

diff_options = {
  boundary_conditions = {
    {
      boundary = e_bndry, type = "vacuum"
    },
    {
      boundary = w_bndry, type = "vacuum"
    },
  },
}

phys1 = diffusion.MGDiffusionSolver.Create({
  name = "mgds",
  residual_tolerance = 1e-8,
  verbose_level = 2
})
diffusion.SetOptions(phys1, diff_options)

solver.Initialize(phys1)
solver.Execute(phys1)
