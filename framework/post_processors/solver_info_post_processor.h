#pragma once

#include "framework/post_processors/post_processor.h"

namespace opensn
{
class App;
}

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
  explicit SolverInfoPostProcessor(opensn::App& app, const InputParameters& params);

  void Execute(const opensn::Event& event_context) override;

private:
  std::shared_ptr<chi_physics::Solver> solver_;
  const ParameterBlock info_;
};

} // namespace chi
