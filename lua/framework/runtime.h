#pragma once

#include <utility>
#include <vector>
#include <string>
#include <stdexcept>
#include <memory>

#include "framework/mpi/mpi.h"
#include "framework/mesh/mesh_handler/mesh_handler.h"
#include "framework/physics/physics_material/physics_material.h"
#include "framework/physics/physics_material/multi_group_xs/multi_group_xs.h"
#include "framework/physics/field_function/field_function.h"
#include "framework/math/spatial_discretization/spatial_discretization.h"
#include "framework/post_processors/post_processor.h"

namespace chi_mesh
{
class MeshHandler;

class SurfaceMesh;
typedef std::shared_ptr<SurfaceMesh> SurfaceMeshPtr;

class FieldFunctionInterpolation;
typedef FieldFunctionInterpolation FFInterp;
typedef std::shared_ptr<FFInterp> FFInterpPtr;

class UnpartitionedMesh;
typedef std::shared_ptr<UnpartitionedMesh> UnpartitionedMeshPtr;
typedef UnpartitionedMeshPtr UnpartMeshPtr;
} // namespace chi_mesh

namespace chi
{

/**Stores all the keys currently in the registries.*/
struct RegistryStatuses
{
  std::vector<std::string> objfactory_keys_;
  std::vector<std::string> console_lua_func_keys_;
  std::vector<std::string> console_lua_wrapper_keys_;
};

} // namespace chi

/**General utilities in ChiTech*/
class Chi
{
public:
  static std::vector<chi_mesh::SurfaceMeshPtr> surface_mesh_stack;
  static std::vector<chi_mesh::FFInterpPtr> field_func_interpolation_stack;
  static std::vector<chi_mesh::UnpartMeshPtr> unpartitionedmesh_stack;

public:
  /// Deleted constructor
  Chi() = delete;
  /// Deleted copy constructor
  Chi(const Chi&) = delete;
  /// Deleted assigment operator
  Chi operator=(const Chi&) = delete;
};

class RecoverableException : public std::runtime_error
{
public:
  explicit RecoverableException(const char* message)
    : std::runtime_error(std::string("RecoverableException: ") + std::string(message))
  {
  }
  explicit RecoverableException(const std::string& message)
    : std::runtime_error(std::string("RecoverableException: ") + message)
  {
  }
  RecoverableException(const std::string& prefix, const std::string& message)
    : std::runtime_error(prefix + message)
  {
  }

  ~RecoverableException() noexcept override = default;
};
