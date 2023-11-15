#pragma once

#include "framework/object.h"

namespace opensn
{
class App;
}

namespace chi_math
{

/**
 * Base class for functions
 *
 */
class Function : public chi::ChiObject
{
public:
  static chi::InputParameters GetInputParameters();

protected:
  explicit Function(const chi::InputParameters& params);
};

typedef std::shared_ptr<Function> FunctionPtr;

} // namespace chi_math
