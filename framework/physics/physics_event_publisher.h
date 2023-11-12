#pragma once

#include "framework/event_system/event_publisher.h"

namespace opensn
{
class App;
}

namespace chi_physics
{

class Solver;

/**A singleton object that can be subscribed to for events.*/
class PhysicsEventPublisher : public chi::EventPublisher
{
public:
  PhysicsEventPublisher(opensn::App& app);

  /// Deleted copy constructor
  PhysicsEventPublisher(const PhysicsEventPublisher&) = delete;
  /// Deleted assignment operator
  PhysicsEventPublisher operator=(const PhysicsEventPublisher&) = delete;

  void PublishEvent(const chi::Event& event) override;

  void SolverInitialize(Solver& solver);
  void SolverExecute(Solver& solver);
  void SolverStep(Solver& solver);
  void SolverAdvance(Solver& solver);
};

} // namespace chi_physics
