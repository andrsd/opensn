#include "framework/math/spatial_discretization/cell_mappings/piecewise_linear_base_mapping.h"
#include "framework/mesh/mesh_continuum/mesh_continuum.h"
#include "framework/app.h"
#include "framework/logging/log.h"

namespace chi_math::cell_mapping
{

PieceWiseLinearBaseMapping::PieceWiseLinearBaseMapping(
  const chi_mesh::MeshContinuum& grid,
  const chi_mesh::Cell& cell,
  size_t num_nodes,
  std::vector<std::vector<int>> face_node_mappings)
  : CellMapping(grid,
                cell,
                num_nodes,
                GetVertexLocations(grid, cell),
                std::move(face_node_mappings),
                &CellMapping::ComputeCellVolumeAndAreas)
{
}

std::vector<std::vector<int>>
PieceWiseLinearBaseMapping::MakeFaceNodeMapping(const chi_mesh::Cell& cell)
{
  const size_t num_faces = cell.faces_.size();
  std::vector<std::vector<int>> mappings;
  mappings.reserve(num_faces);
  for (auto& face : cell.faces_)
  {
    std::vector<int> face_dof_mapping;
    face_dof_mapping.reserve(face.vertex_ids_.size());
    for (uint64_t fvid : face.vertex_ids_)
    {
      int mapping = -1;
      for (size_t ci = 0; ci < cell.vertex_ids_.size(); ci++)
      {
        if (fvid == cell.vertex_ids_[ci])
        {
          mapping = static_cast<int>(ci);
          break;
        }
      } // for cell i
      if (mapping < 0)
      {
        // FIXME: make this work
        // Chi::log.LogAllError() << "Unknown face mapping encountered. pwl_polyhedron.h";
        opensn::App::Exit(EXIT_FAILURE);
      }
      face_dof_mapping.push_back(mapping);
    } // for face i

    mappings.push_back(face_dof_mapping);
  }
  return mappings;
}

std::vector<chi_mesh::Vector3>
PieceWiseLinearBaseMapping::GetVertexLocations(const chi_mesh::MeshContinuum& grid,
                                               const chi_mesh::Cell& cell)
{
  std::vector<chi_mesh::Vector3> verts;
  verts.reserve(cell.vertex_ids_.size());

  for (const auto vid : cell.vertex_ids_)
    verts.push_back(grid.vertices[vid]);

  return verts;
}

} // namespace chi_math::cell_mapping
