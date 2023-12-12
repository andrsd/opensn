# Boundary Conditions

LBS supports the boundary condition `"incident_anisotropic_heterogeneous"`,
which allows for a very intricate type of boundary condition to be specified.

For example, consider the boundary condition applied to boundary `zmax` below:
```
LBSSetOptions(phys1,
{
  spatial_discretization = "pwld",
  scattering_order = 2,
  boundary_conditions =
  {
    { name = "zmax", type = "incident_anisotropic_heterogeneous",
      function_name = "luaBoundaryFunctionA"},
  }
})
```

It points to the lua function `"luaBoundaryFunctionA"`. This lua function will
be called with the following parameters:
```
size_t        cell_global_id,
int           cell_material_id,
unsigned int  face_index,
unsigned int  face_node_index,
const chi_mesh::Vector3 face_node_location,
const chi_mesh::Vector3 face_node_normal,
const std::vectorint quadrature_angle_indices,
const std::vectorchi_mesh::Vector3 quadrature_angle_vectors,
const std::vectorstd::pairdouble,double quadrature_phi_theta_angles,
const std::vectorint group_indices,
double evaluation_time;
```
and must return a 1D array of data-values ordered first by angle index, then
by group index, e.g., n0g0, n0g1, n0g2, n1g0, n1g1, n1g2, etc.

Example lua function:
```
function luaBoundaryFunctionA(cell_global_id,
                              material_id,
                              location,
                              normal,
                              quadrature_angle_indices,
                              quadrature_angle_vectors,
                              quadrature_phi_theta_angles,
                              group_indices)
    num_angles = rawlen(quadrature_angle_vectors)
    num_groups = rawlen(group_indices)
    psi = {}
    dof_count = 0

    for ni=1,num_angles do
        omega = quadrature_angle_vectorsni
        phi_theta = quadrature_phi_theta_anglesni
        for gi=1,num_groups do
            g = group_indicesgi

            value = 1.0

            dof_count = dof_count + 1
            psidof_count = value
        end
    end

    return psi
end
```

An example of a very intricate use of this functionality can be seen in the
test [tests_Transport_Steady_Transport2D_5PolyA_AniHeteroBndry_lua](<TODO>)
