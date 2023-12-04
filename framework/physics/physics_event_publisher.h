#pragma once

#include "framework/event_system/event_publisher.h"

namespace opensn
{

class Solver;

/**A singleton object that can be subscribed to for events.*/
class PhysicsEventPublisher : public EventPublisher
{
public:
  /**Access to the singleton*/
  static PhysicsEventPublisher& GetInstance();
  /// Deleted copy constructor
  PhysicsEventPublisher(const PhysicsEventPublisher&) = delete;
  PhysicsEventPublisher
  /// Deleted assignment operator
  operator=(const PhysicsEventPublisher&) = delete;

  void PublishEvent(const Event& event) override;

  void SolverInitialize(Solver& solver);
  void SolverExecute(Solver& solver);
  void SolverStep(Solver& solver);
  void SolverAdvance(Solver& solver);

private:
  PhysicsEventPublisher();
};

} // namespace opensn
