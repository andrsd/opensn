#pragma once

#include "opensn/framework/ChiObject.h"
#include "opensn/framework/math/chi_math_time_stepping.h"

namespace chi_math
{

class TimeIntegration : public ChiObject
{
private:
  SteppingMethod method_;

public:
  static chi::InputParameters GetInputParameters();
  explicit TimeIntegration(const chi::InputParameters& params);

  SteppingMethod Method() const;

  virtual ~TimeIntegration() = default;
};

} // namespace chi_math
