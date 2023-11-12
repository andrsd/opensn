#pragma once

#include "framework/logging/log.h"
#include "framework/object.h"
#include "framework/object_factory.h"
#include "framework/utils/timer.h"
#include "framework/math/spatial_discretization/spatial_discretization.h"
#include "framework/mesh/mesh_handler/mesh_handler.h"
#include "framework/physics/physics_material/physics_material.h"
#include "framework/physics/physics_event_publisher.h"
#include "framework/physics/field_function/field_function.h"
#include "framework/physics/physics_material/multi_group_xs/multi_group_xs.h"
#include "framework/post_processors/post_processor.h"
#include <mpi.h>

namespace opensn
{

class App
{
public:
  explicit App(MPI_Comm comm);

  MPI_Comm Comm() const;
  const int& LocationID() const;
  const int& ProcessCount() const;

  chi::ChiLog& Log();
  std::vector<chi::ChiObjectPtr>& ObjectStack();
  std::vector<chi_mesh::MeshHandlerPtr>& MeshHandlerStack();
  std::vector<chi_physics::MaterialPtr>& MaterialStack();
  std::vector<chi_math::SpatialDiscretizationPtr>& SdmStack();
  std::vector<chi_physics::FieldFunctionPtr>& FieldFunctionStack();
  std::vector<chi_physics::MultiGroupXSPtr>& MultigroupXSStack();
  std::vector<chi::PostProcessorPtr>& PostprocessorStack();
  std::vector<chi_math::AngularQuadraturePtr>& AngularQuadratureStack();
  chi_physics::PhysicsEventPublisher& PhysicsEventPublisher();

  chi::Timer& ProgramTimer();

  ChiObjectFactory& Factory();

  /**
   * Obtains a reference to the current mesh handler from the global stack.
   *
   * If the stack is empty this routine will through `std::logic_error`.
   */
  int CurrentMeshHandler() const;

  std::shared_ptr<chi_mesh::MeshHandler> GetCurrentMeshHandler();

  /**
   * Adds a new mesh handler to the stack, sets it as the current handler and returns a handle to
   * it.
   */
  size_t PushNewHandlerAndGetIndex();

  template <class T>
  std::shared_ptr<T> GetStackObject(const size_t handle,
                                    const std::string& calling_function_name = "Unknown")
  {
    return GetStackItem<T>(object_stack_, handle, calling_function_name);
  }

  chi_physics::MaterialPtr GetMaterial(const size_t handle,
                                       const std::string& calling_function_name = "Unknown")
  {
    return GetStackItem<chi_physics::Material>(material_stack_, handle, calling_function_name);
  }

  chi_physics::FieldFunctionPtr
  GetFieldFunction(const size_t handle, const std::string& calling_function_name = "Unknown")
  {
    return GetStackItem<chi_physics::FieldFunction>(
      field_function_stack_, handle, calling_function_name);
  }

  chi_physics::MultiGroupXSPtr GetMultiGroupXS(const size_t handle,
                                               const std::string& calling_function_name = "Unknown")
  {
    return GetStackItem<chi_physics::MultiGroupXS>(
      multigroup_xs_stack_, handle, calling_function_name);
  }

  chi_math::AngularQuadraturePtr
  GetAngularQuadrature(const size_t handle, const std::string& calling_function_name = "Unknown")
  {
    return GetStackItem<chi_math::AngularQuadrature>(
      angular_quadrature_stack_, handle, calling_function_name);
  }

  void Barrier() const;

protected:
  template <class R, class T>
  std::shared_ptr<R> GetStackItem(std::vector<std::shared_ptr<T>>& stack,
                                  const size_t handle,
                                  const std::string& calling_function_name = "Unknown")
  {
    try
    {
      std::shared_ptr<T> item = stack.at(handle);
      std::shared_ptr<R> ret_item = std::dynamic_pointer_cast<R>(item);
      if (not ret_item)
        throw std::logic_error("chi::GetStackItem: Invalid return type used. Calling function: " +
                               calling_function_name);
      return ret_item;
    }
    catch (const std::out_of_range& oor)
    {
      throw std::out_of_range("chi::GetStackItem: Invalid handle used. Calling function: " +
                              calling_function_name);
    }
  }

private:
  /// MPI communicator this app run on
  MPI_Comm comm_;
  //
  int location_id_;
  //
  int proccess_count_;

  /// Logging object
  chi::ChiLog log_;

  /// Factory for creating objects
  ChiObjectFactory factory_;

  /// Objects
  std::vector<chi::ChiObjectPtr> object_stack_;
  /// Mesh handlers
  std::vector<chi_mesh::MeshHandlerPtr> mesh_handler_stack_;
  /// Spatial discretizations
  std::vector<chi_math::SpatialDiscretizationPtr> sdm_stack_;
  /// Field functions
  std::vector<chi_physics::FieldFunctionPtr> field_function_stack_;
  /// Matrials
  std::vector<chi_physics::MaterialPtr> material_stack_;
  /// Multi-group XSs
  std::vector<chi_physics::MultiGroupXSPtr> multigroup_xs_stack_;
  /// Post-processors
  std::vector<chi::PostProcessorPtr> postprocessor_stack_;
  /// Angular quadratures
  std::vector<chi_math::AngularQuadraturePtr> angular_quadrature_stack_;

  /// Pysics event publisher
  chi_physics::PhysicsEventPublisher physics_event_publisher_;

  /// Program timer
  chi::Timer program_timer_;

  int current_mesh_handler_;

public:
  /** Exits the program appropriately.*/
  static void Exit(int error_code);
};

} // namespace opensn
