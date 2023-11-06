#pragma once

#include "framework/post_processors/PostProcessor.h"

namespace chi_physics
{
class Solver;
}

namespace chi
{

class SolverInfoPostProcessor : public PostProcessor
{
public:
  static InputParameters GetInputParameters();
  explicit SolverInfoPostProcessor(const InputParameters& params);

  void Execute(const Event& event_context) override;

private:
  const chi_physics::Solver& solver_;
  const ParameterBlock info_;
};

} // namespace chi
