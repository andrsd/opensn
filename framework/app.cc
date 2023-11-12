#include "app.h"

namespace opensn
{

App::App(MPI_Comm comm) : comm_(comm), log_(*this), factory_(*this), physics_event_publisher_(*this)
{
  MPI_Comm_rank(comm_, &location_id_);
  MPI_Comm_size(comm_, &proccess_count_);
}

MPI_Comm
App::Comm() const
{
  return comm_;
}

const int&
App::LocationID() const
{
  return location_id_;
}

const int&
App::ProcessCount() const
{
  return proccess_count_;
}

chi::ChiLog&
App::Log()
{
  return log_;
}

std::vector<chi::ChiObjectPtr>&
App::ObjectStack()
{
  return object_stack_;
}

std::vector<chi_mesh::MeshHandlerPtr>&
App::MeshHandlerStack()
{
  return mesh_handler_stack_;
}

std::vector<chi_physics::MaterialPtr>&
App::MaterialStack()
{
  return material_stack_;
}

std::vector<chi_math::SpatialDiscretizationPtr>&
App::SdmStack()
{
  return sdm_stack_;
}

std::vector<chi_physics::FieldFunctionPtr>&
App::FieldFunctionStack()
{
  return field_function_stack_;
}

std::vector<chi_physics::MultiGroupXSPtr>&
App::MultigroupXSStack()
{
  return multigroup_xs_stack_;
}

std::vector<chi::PostProcessorPtr>&
App::PostprocessorStack()
{
  return postprocessor_stack_;
}

std::vector<chi_math::AngularQuadraturePtr>&
App::AngularQuadratureStack()
{
  return angular_quadrature_stack_;
}

chi_physics::PhysicsEventPublisher&
App::PhysicsEventPublisher()
{
  return physics_event_publisher_;
}

chi::Timer&
App::ProgramTimer()
{
  return program_timer_;
}

ChiObjectFactory&
App::Factory()
{
  return factory_;
}

int
App::CurrentMeshHandler() const
{
  return current_mesh_handler_;
}

void
App::Barrier() const
{
  MPI_Barrier(this->comm_);
}

void
App::Exit(int error_code)
{
  // FIXME: use the real comm
  MPI_Abort(MPI_COMM_WORLD, error_code);
}

std::shared_ptr<chi_mesh::MeshHandler>
App::GetCurrentMeshHandler()
{
  if (mesh_handler_stack_.empty())
    throw std::logic_error("chi_mesh::GetCurrentHandler: No handlers on stack");

  return GetStackItem<chi_mesh::MeshHandler>(mesh_handler_stack_, current_mesh_handler_);
}

size_t
App::PushNewHandlerAndGetIndex()
{
  mesh_handler_stack_.push_back(std::make_shared<chi_mesh::MeshHandler>());

  int index = static_cast<int>(mesh_handler_stack_.size() - 1);
  current_mesh_handler_ = index;

  return index;
}

} // namespace opensn
