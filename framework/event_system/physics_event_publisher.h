#pragma once

#include "framework/event_system/event_publisher.h"

namespace chi_physics
{
class Solver;
}

namespace opensn
{
class App;

/**A singleton object that can be subscribed to for events.*/
class PhysicsEventPublisher : public EventPublisher
{
public:
  PhysicsEventPublisher(opensn::App& app);

  /// Deleted copy constructor
  PhysicsEventPublisher(const PhysicsEventPublisher&) = delete;
  /// Deleted assignment operator
  PhysicsEventPublisher operator=(const PhysicsEventPublisher&) = delete;

  void PublishEvent(const Event& event) override;

  void SolverInitialize(chi_physics::Solver& solver);
  void SolverExecute(chi_physics::Solver& solver);
  void SolverStep(chi_physics::Solver& solver);
  void SolverAdvance(chi_physics::Solver& solver);
};

} // namespace opensn
