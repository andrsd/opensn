nodes = {}
N = 3
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

xss = {}
xss[1] = xs.Create()
xs.Set(xss[1], OPENSN_XSFILE, "materials/XS_UO2.xs")

materials = {}
materials[1] = mat.AddMaterial("Test Material")
mat.SetProperty(materials[1], TRANSPORT_XSECTIONS, EXISTING, xss[1])

-- mat.SetProperty(materials[1], TRANSPORT_XSECTIONS, SIMPLE_ONE_GROUP, 1., 0.9)

num_groups = 2

-- Setup Physics

-- Angular quadrature
-- pquad = aquad.CreateProductQuadrature(GAUSS_LEGENDRE_CHEBYSHEV, 2, 2)
pquad = aquad.CreateProductQuadrature(GAUSS_LEGENDRE, 2)
-- aquad.OptimizeForPolarSymmetry(pquad, 4.0 * math.pi)

-- Solver
-- if string.find(k_method, "scdsa") or string.find(k_method, "smm") then
--   inner_linear_method = "krylov_richardson"
--   l_max_its = 1
-- else
inner_linear_method = "gmres"
l_max_its = 5
-- end

phys1 = lbs.DiscreteOrdinatesSolver.Create({
  num_groups = num_groups,
  groupsets = {
    {
      groups_from_to = { 0, num_groups - 1 },
      angular_quadrature_handle = pquad,
      inner_linear_method = inner_linear_method,
      -- l_max_its = l_max_its,
      -- l_abs_tol = 1.0e-10,
      -- angle_aggregation_type = "polar",
      -- angle_aggregation_num_subsets = 1,
      -- groupset_num_subsets = 1,
      apply_wgdsa = false,
      wgdsa_l_abs_tol = 1.0e-9,
    },
  },
  options = {
    boundary_conditions = {
      -- { name = "zmin", type = "vacuum" },
      -- { name = "zmax", type = "vacuum" },
      { name = "zmin", type = "reflecting" },
      { name = "zmax", type = "reflecting" },
    },
    scattering_order = 1,
    verbose_outer_iterations = true,
    verbose_inner_iterations = true,
    -- power_field_function_on = true,
    -- power_default_kappa = 1.0,
    -- power_normalization = 1.0,
    save_angular_flux = true,
  },
  sweep_type = "AAH",
})

-- Execute Solver
k_solver = lbs.NonLinearKEigen.Create({
  lbs_solver_handle = phys1,
  nl_max_its = 50,
  nl_abs_tol = 1.0e-13,
  nl_rel_tol = 1.0e-13,
  l_rel_tol = 1.0e-5,
  -- l_max_its = 20,
  -- num_initial_power_iterations = 2,
})

solver.Initialize(k_solver)
solver.Execute(k_solver)
