#pragma once

#include "framework/math/time_integrations/theta_scheme_time_intgr.h"

namespace chi_math
{

class CrankNicolsonTimeIntegration : public ThetaSchemeTimeIntegration
{
public:
  static chi::InputParameters GetInputParameters();
  explicit CrankNicolsonTimeIntegration(opensn::App& app, const chi::InputParameters& params);
};

} // namespace chi_math
