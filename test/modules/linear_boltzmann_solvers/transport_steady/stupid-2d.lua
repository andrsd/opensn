nodesx = {}
Nx = 1
Lx = 0.8
xmin = -Lx / 2
dx = Lx / Nx
for i = 1, (Nx + 1) do
    k = i - 1
    nodesx[i] = xmin + k * dx
end

nodesy = {}
Ny = 1
Ly = 0.8
ymin = -Ly / 2
dy = Ly / Ny
for i = 1, (Ny + 1) do
    k = i - 1
    nodesy[i] = ymin + k * dy
end

meshgen1 = mesh.OrthogonalMeshGenerator.Create({ node_sets = { nodesx, nodesy } })
mesh.MeshGenerator.Execute(meshgen1)

-- Set Material IDs
vol0 = logvol.RPPLogicalVolume.Create({ infx = true, infy = true, infz = true })
mesh.SetMaterialIDFromLogicalVolume(vol0, 0)

-- Add materials
materials = {}
materials[1] = mat.AddMaterial("Test Material")

num_groups = 1
mat.SetProperty(materials[1], TRANSPORT_XSECTIONS, SIMPLE_ONE_GROUP, 0., 0.)

-- src = {}
-- for g = 1, num_groups do
--   src[g] = 0.0
-- end
-- --src[1] = 1.0
-- mat.SetProperty(materials[1], ISOTROPIC_MG_SOURCE, FROM_ARRAY, src)

-- Setup Physics
pquad0 = aquad.CreateProductQuadrature(GAUSS_LEGENDRE_CHEBYSHEV, 2, 1)
aquad.OptimizeForPolarSymmetry(pquad0, 4.0 * math.pi)

lbs_block = {
    num_groups = num_groups,
    groupsets = {
        {
            groups_from_to = { 0, 0 },
            angular_quadrature_handle = pquad0,
            angle_aggregation_num_subsets = 1,
            groupset_num_subsets = 2,
            inner_linear_method = "gmres",
            l_abs_tol = 1.0e-6,
            l_max_its = 300,
            gmres_restart_interval = 100,
        }
    },
}

lbs_options = {
    boundary_conditions = {
        {
            name = "xmin",
            type = "vacuum",
        },
        {
            name = "xmax",
            type = "vacuum",
        },
    },
    scattering_order = 1,
    max_ags_iterations = 1,
}

phys1 = lbs.DiscreteOrdinatesSolver.Create(lbs_block)
lbs.SetOptions(phys1, lbs_options)

-- Initialize and Execute Solver
ss_solver = lbs.SteadyStateSolver.Create({ lbs_solver_handle = phys1 })

solver.Initialize(ss_solver)
solver.Execute(ss_solver)
