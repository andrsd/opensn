#include "framework/event_system/physics_event_publisher.h"
#include "framework/app.h"
#include "framework/event_system/event.h"
#include "framework/event_system/system_wide_event_publisher.h"
#include "framework/physics/solver_base/solver.h"
#include "framework/physics/time_steppers/time_stepper.h"

namespace opensn
{

PhysicsEventPublisher::PhysicsEventPublisher(opensn::App& app) : EventPublisher(app, "Physics")
{
}

void
PhysicsEventPublisher::PublishEvent(const Event& event)
{
  EventPublisher::PublishEvent(event);
  App().SystemWideEventPublisher().PublishEvent(event);
}

void
PhysicsEventPublisher::SolverInitialize(chi_physics::Solver& solver)
{
  {
    const std::string event_name = "SolverPreInitialize";
    chi::ParameterBlock params;
    params.AddParameter("solver_name", solver.TextName());
    params.AddParameter("solver_handle", solver.StackID());

    PublishEvent(Event(event_name, params));
  }

  solver.Initialize();

  {
    const std::string event_name = "SolverInitialized";
    chi::ParameterBlock params;
    params.AddParameter("solver_name", solver.TextName());
    params.AddParameter("solver_handle", solver.StackID());
    params.AddParameter("time", solver.GetTimeStepper().Time());

    PublishEvent(Event(event_name, params));
  }
}

void
PhysicsEventPublisher::SolverExecute(chi_physics::Solver& solver)
{
  {
    const std::string event_name = "SolverPreExecution";
    chi::ParameterBlock params;
    params.AddParameter("solver_name", solver.TextName());
    params.AddParameter("solver_handle", solver.StackID());

    PublishEvent(Event(event_name, params));
  }

  solver.Execute();

  {
    const std::string event_name = "SolverExecuted";
    chi::ParameterBlock params;
    params.AddParameter("solver_name", solver.TextName());
    params.AddParameter("solver_handle", solver.StackID());

    PublishEvent(Event(event_name, params));
  }
}

void
PhysicsEventPublisher::SolverStep(chi_physics::Solver& solver)
{
  {
    const std::string event_name = "SolverPreStep";
    chi::ParameterBlock params;
    params.AddParameter("solver_name", solver.TextName());
    params.AddParameter("solver_handle", solver.StackID());

    PublishEvent(Event(event_name, params));
  }

  solver.Step();

  {
    const std::string event_name = "SolverStep";
    chi::ParameterBlock params;
    params.AddParameter("solver_name", solver.TextName());
    params.AddParameter("solver_handle", solver.StackID());

    PublishEvent(Event(event_name, params));
  }
}

void
PhysicsEventPublisher::SolverAdvance(chi_physics::Solver& solver)
{
  {
    const std::string event_name = "SolverPreAdvance";
    chi::ParameterBlock params;
    params.AddParameter("solver_name", solver.TextName());
    params.AddParameter("solver_handle", solver.StackID());

    PublishEvent(Event(event_name, params));
  }

  solver.Advance();

  {
    const std::string event_name = "SolverAdvanced";
    chi::ParameterBlock params;
    params.AddParameter("solver_name", solver.TextName());
    params.AddParameter("solver_handle", solver.StackID());
    params.AddParameter("time", solver.GetTimeStepper().Time());
    params.AddParameter("timestep_index", solver.GetTimeStepper().TimeStepIndex());

    PublishEvent(Event(event_name, params));
  }
}

} // namespace opensn
