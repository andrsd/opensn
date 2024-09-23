-- Create Mesh
widths = { 2. }
nrefs = { 4 }

widths = { 2 }
nrefs = { 3 }

-- Nmat = #widths
Nmat = 1

nodes = {}
counter = 1
nodes[counter] = 0.
for imat = 1, 1 do
    dx = widths[imat] / nrefs[imat]
    for i = 1, nrefs[imat] do
        counter = counter + 1
        nodes[counter] = nodes[counter - 1] + dx
    end
end

meshgen = mesh.OrthogonalMeshGenerator.Create({ node_sets = { nodes } })
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
for imat = 1, Nmat do
    materials[imat] = mat.AddMaterial(mat_names[imat])
end

-- Add cross sections to materials
total = { 0. }
c = { 0. }
for imat = 1, Nmat do
    mat.SetProperty(materials[1], TRANSPORT_XSECTIONS, OPENSN_XSFILE, "response_2d_3_mat1.xs")
    -- mat.SetProperty(materials[imat], TRANSPORT_XSECTIONS, SIMPLE_ONE_GROUP, total[imat], c[imat])
end

-- Create sources in 1st and 4th materials
mat.SetProperty(materials[1], ISOTROPIC_MG_SOURCE, FROM_ARRAY, { 5., 0, 0, 0, 0, 0, 0, 0, 0, 7. })
-- mat.SetProperty(materials[4], ISOTROPIC_MG_SOURCE, FROM_ARRAY, { 1. })

-- Angular Quadrature
gl_quad = aquad.CreateProductQuadrature(GAUSS_LEGENDRE, 4)
-- gl_quad = aquad.CreateProductQuadrature(GAUSS_LEGENDRE_LEGENDRE, 2, 4)

-- LBS block option
num_groups = 10
lbs_block = {
    num_groups = num_groups,
    sweep_type = "CBC",
    groupsets = {
        {
            groups_from_to = { 0, 3 },
            angular_quadrature_handle = gl_quad,
            inner_linear_method = "gmres",
            l_abs_tol = 1.0e-9,
            l_max_its = 300,
            gmres_restart_interval = 30,
        },
        {
            groups_from_to = { 4, 9 },
            angular_quadrature_handle = gl_quad,
            inner_linear_method = "gmres",
            l_abs_tol = 1.0e-9,
            l_max_its = 300,
            gmres_restart_interval = 30,
        }
    },
    options = {
        scattering_order = 0,
        save_angular_flux = true,
        spatial_discretization = "pwld",
        boundary_conditions = { { name = "zmin", type = "vacuum" }, { name = "zmax", type = "vacuum" } },
    },
}

phys = lbs.DiscreteOrdinatesSolver.Create(lbs_block)

-- Initialize and execute solver
ss_solver = lbs.SteadyStateSolver.Create({ lbs_solver_handle = phys })

solver.Initialize(ss_solver)
solver.Execute(ss_solver)
