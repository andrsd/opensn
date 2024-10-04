-- 1D 1G KEigenvalue::Solver test using power iteration

N = 40
L = 14

--############################################### Setup mesh
nodes = {}
dx = L / N
for i = 0, N do
    nodes[i + 1] = i * dx
end

meshgen1 = mesh.OrthogonalMeshGenerator.Create({ node_sets = { nodes } })
mesh.MeshGenerator.Execute(meshgen1)

-- Set Material IDs
mesh.SetUniformMaterialID(0)

vol1 = logvol.RPPLogicalVolume.Create({
    xmin = -1000.0,
    xmax = 10.0,
    ymin = -1000.0,
    ymax = 10.0,
    infz = true,
})
mesh.SetMaterialIDFromLogicalVolume(vol1, 1)

--############################################### Add materials
materials = {}
materials[1] = mat.AddMaterial("water")
materials[2] = mat.AddMaterial("fuel")

mat.SetProperty(materials[1], TRANSPORT_XSECTIONS, OPENSN_XSFILE, "xs_water_g2.xs")
mat.SetProperty(materials[2], TRANSPORT_XSECTIONS, OPENSN_XSFILE, "xs_fuel_g2.xs")

--############################################### Setup Physics
aquad = aquad.CreateProductQuadrature(GAUSS_LEGENDRE, 16)

num_groups = 2
lbs_block = {
    num_groups = num_groups,
    groupsets = {
        {
            groups_from_to = { 0, num_groups - 1 },
            angular_quadrature_handle = aquad,
            inner_linear_method = "gmres",
            l_max_its = 50,
            gmres_restart_interval = 50,
            l_abs_tol = 1.0e-10,
            groupset_num_subsets = 2,
        },
    },
    options = {
        boundary_conditions = {
            { name = "xmin", type = "reflecting" },
        },
        scattering_order = 2,
        use_precursors = false,

        verbose_inner_iterations = false,
        verbose_outer_iterations = true,
        save_angular_flux = true,
    },
    sweep_type = "CBC",
}

phys = lbs.DiscreteOrdinatesSolver.Create(lbs_block)

-- k_solver0 = lbs.NonLinearKEigen.Create({
--   lbs_solver_handle = phys,
--   nl_max_its = kes_max_iterations,
--   nl_abs_tol = kes_tolerance,
-- })
k_solver0 = lbs.PowerIterationKEigen.Create({ lbs_solver_handle = phys })
solver.Initialize(k_solver0)
solver.Execute(k_solver0)

--############################################### Get field functions
--############################################### Line plot
--############################################### Volume integrations
--############################################### Exports
--############################################### Plots
