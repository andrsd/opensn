meshgen = mesh.FromFileMeshGenerator.Create({
    -- filename = "test-2d.obj",
    filename = "square-tri.e",
})
mesh.MeshGenerator.Execute(meshgen)

-- Set Material IDs
-- z_min = 0.0
-- z_max = widths[1]
-- for imat = 1, Nmat do
--     z_max = z_min + widths[imat]
--     log.Log(LOG_0, "imat=" .. imat .. ", zmin=" .. z_min .. ", zmax=" .. z_max)
--     lv = logvol.RPPLogicalVolume.Create({ infx = true, infy = true, zmin = z_min, zmax = z_max })
--     mesh.SetMaterialIDFromLogicalVolume(lv, imat - 1)
--     z_min = z_max
-- end
mesh.SetUniformMaterialID(0)

-- Create materials
mat_names = { "void" }
materials = {}
materials[1] = mat.AddMaterial("void")

-- Add cross sections to materials
mat.SetProperty(materials[1], TRANSPORT_XSECTIONS, SIMPLE_ONE_GROUP, 0., 0.)

-- Create sources in 1st and 4th materials
-- mat.SetProperty(materials[1], ISOTROPIC_MG_SOURCE, FROM_ARRAY, { 50. })
-- mat.SetProperty(materials[4], ISOTROPIC_MG_SOURCE, FROM_ARRAY, { 1. })

mat.SetProperty(materials[1], ISOTROPIC_MG_SOURCE, FROM_ARRAY, { 12. })

-- Angular Quadrature
gl_quad = aquad.CreateProductQuadrature(GAUSS_LEGENDRE, 4)
-- gl_quad = aquad.CreateProductQuadrature(GAUSS_LEGENDRE_LEGENDRE, 2, 4)

-- LBS block option
num_groups = 1
lbs_block = {
    num_groups = num_groups,
    sweep_type = "CBC",
    groupsets = {
        {
            groups_from_to = { 0, num_groups - 1 },
            angular_quadrature_handle = gl_quad,
            inner_linear_method = "gmres",
            l_abs_tol = 1.0e-9,
            l_max_its = 300,
            gmres_restart_interval = 30,
        },
    },
    options = {
        scattering_order = 0,
        save_angular_flux = true,
        spatial_discretization = "pwld",
        -- boundary_conditions = { { name = "left", type = "vacuum" }, { name = "zmax", type = "vacuum" } },
    },
}

phys = lbs.DiscreteOrdinatesSolver.Create(lbs_block)

-- Initialize and execute solver
ss_solver = lbs.SteadyStateSolver.Create({ lbs_solver_handle = phys })

solver.Initialize(ss_solver)
solver.Execute(ss_solver)

lbs.ComputeBalance(phys)
