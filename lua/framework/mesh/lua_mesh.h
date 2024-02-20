#pragma once

#include <vector>

namespace opensn
{
class MeshContinuum;
}

namespace opensnlua
{

std::size_t AddMeshContinuum(std::shared_ptr<opensn::MeshContinuum> mesh);

std::shared_ptr<opensn::MeshContinuum> MeshContinuumFromHandle(std::size_t handle);

} // namespace opensnlua
