-- Setup mesh
nodes = {}
N = 2
L = 2
xmin = -L / 2
dx = L / N
for i = 1, (N + 1) do
  k = i - 1
  nodes[i] = xmin + k * dx
end

meshgen1 = mesh.OrthogonalMeshGenerator.Create({ node_sets = { nodes, nodes } })
mesh.MeshGenerator.Execute(meshgen1)

-- Set Material IDs
mesh.SetUniformMaterialID(0)

-- Setboundary IDs
-- xmin,xmax,ymin,ymax,zmin,zmax
e_vol = logvol.RPPLogicalVolume.Create({ xmin = 0.99999, xmax = 1000.0, infy = true, infz = true })
w_vol =
    logvol.RPPLogicalVolume.Create({ xmin = -1000.0, xmax = -0.99999, infy = true, infz = true })
n_vol = logvol.RPPLogicalVolume.Create({ ymin = 0.99999, ymax = 1000.0, infx = true, infz = true })
s_vol =
    logvol.RPPLogicalVolume.Create({ ymin = -1000.0, ymax = -0.99999, infx = true, infz = true })

e_bndry = "0"
w_bndry = "1"
n_bndry = "2"
s_bndry = "3"

mesh.SetBoundaryIDFromLogicalVolume(e_vol, e_bndry)
mesh.SetBoundaryIDFromLogicalVolume(w_vol, w_bndry)
mesh.SetBoundaryIDFromLogicalVolume(n_vol, n_bndry)
mesh.SetBoundaryIDFromLogicalVolume(s_vol, s_bndry)

-- materials
materials = {}
materials[1] = mat.AddMaterial("Test Material")
-- materials[2] = mat.AddMaterial("Test Material2")

-- num_groups = 168
-- mat.SetProperty(materials[1], TRANSPORT_XSECTIONS, OPENSN_XSFILE, "xs_3_170.xs")
-- mat.SetProperty(materials[2], TRANSPORT_XSECTIONS, OPENSN_XSFILE, "xs_3_170.xs")
num_groups = 1
mat.SetProperty(materials[1], TRANSPORT_XSECTIONS, SIMPLE_ONE_GROUP, 1., 0.9)

src = {}
for g = 1, num_groups do
  src[g] = 0.0
end
src[1] = 1.0
mat.SetProperty(materials[1], ISOTROPIC_MG_SOURCE, FROM_ARRAY, src)
-- mat.SetProperty(materials[2], ISOTROPIC_MG_SOURCE, FROM_ARRAY, src)

diff_options = {
  boundary_conditions = {
    {
      boundary = e_bndry, type = "vacuum"
    },
    {
      boundary = n_bndry, type = "vacuum"
    },
    {
      boundary = s_bndry, type = "vacuum"
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
