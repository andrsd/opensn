#include "framework/mesh/mesh_continuum/mesh_continuum.h"
#include "framework/runtime.h"
#include "framework/logging/log_exceptions.h"

namespace opensnlua
{

// Meshes produced by mesh generators
std::vector<std::shared_ptr<opensn::MeshContinuum>> mesh_stack;

std::size_t
AddMeshContinuum(std::shared_ptr<opensn::MeshContinuum> mesh)
{
  auto handle = mesh_stack.size();
  mesh_stack.push_back(mesh);
  opensn::mesh_stack.push_back(mesh);
  return handle;
}

std::shared_ptr<opensn::MeshContinuum>
MeshContinuumFromHandle(std::size_t handle)
{
  if (handle < mesh_stack.size())
    return mesh_stack.at(handle);
  else
    OpenSnInvalidArgument("Invalid mesh handle.");
}

} // namespace opensnlua
