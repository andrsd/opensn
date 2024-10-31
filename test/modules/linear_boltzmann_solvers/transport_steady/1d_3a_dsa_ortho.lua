nodes = {}
N = 5
L = 1
xmin = -L / 2
dx = L / N
for i = 1, (N + 1) do
  k = i - 1
  nodes[i] = xmin + k * dx
end

meshgen1 = mesh.OrthogonalMeshGenerator.Create({ node_sets = { nodes } })
mesh.MeshGenerator.Execute(meshgen1)

mesh.SetUniformMaterialID(0)

-- vol1 = logvol.RPPLogicalVolume.Create({ infx = true, infy = true, zmin = -0.2, zmax = 0.2 })
-- mesh.SetMaterialIDFromLogicalVolume(vol1, 1)

materials = {}
materials[1] = mat.AddMaterial("Test Material")
-- materials[2] = mat.AddMaterial("Test Material2")

-- num_groups = 2
-- mat.SetProperty(materials[1], TRANSPORT_XSECTIONS, OPENSN_XSFILE, "xs_graphite_pure.xs")
-- mat.SetProperty(materials[2], TRANSPORT_XSECTIONS, OPENSN_XSFILE, "xs_air50RH.xs")

num_groups = 1
mat.SetProperty(materials[1], TRANSPORT_XSECTIONS, SIMPLE_ONE_GROUP, 1., 0.9)
-- mat.SetProperty(materials[2], TRANSPORT_XSECTIONS, SIMPLE_ONE_GROUP, 1., 0.9)

src = {}
for g = 1, num_groups do
  src[g] = 0.0
end
src[1] = 1.0
mat.SetProperty(materials[1], ISOTROPIC_MG_SOURCE, FROM_ARRAY, src)
-- src[1] = 0.0
-- mat.SetProperty(materials[2], ISOTROPIC_MG_SOURCE, FROM_ARRAY, src)

pquad0 = aquad.CreateProductQuadrature(GAUSS_LEGENDRE_CHEBYSHEV, 2, 2, false)

lbs_block = {
  num_groups = num_groups,
  groupsets = {
    {
      groups_from_to = { 0, num_groups - 1 },
      angular_quadrature_handle = pquad0,
      angle_aggregation_num_subsets = 1,
      groupset_num_subsets = 1,
      inner_linear_method = "gmres",
      l_abs_tol = 1.0e-6,
      l_max_its = 1000,
      gmres_restart_interval = 30,
      apply_wgdsa = true,
      wgdsa_l_abs_tol = 1.0e-2,
    }
  },
}

lbs_options = {
  scattering_order = 0,
  max_ags_iterations = 1,
  boundary_conditions = {
      { name = "zmin", type = "vacuum" },
      { name = "zmax", type = "vacuum" }
  },
}

phys1 = lbs.DiscreteOrdinatesSolver.Create(lbs_block)
lbs.SetOptions(phys1, lbs_options)

ss_solver = lbs.SteadyStateSolver.Create({ lbs_solver_handle = phys1 })
solver.Initialize(ss_solver)
solver.Execute(ss_solver)
