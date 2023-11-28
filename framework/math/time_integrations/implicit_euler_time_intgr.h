#pragma once

#include "framework/math/time_integrations/theta_scheme_time_intgr.h"

namespace opensn
{

class ImplicitEulerTimeIntegration : public ThetaSchemeTimeIntegration
{
public:
  static InputParameters GetInputParameters();
  explicit ImplicitEulerTimeIntegration(const InputParameters& params);
};

} // namespace opensn
