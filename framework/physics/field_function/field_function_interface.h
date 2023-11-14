#pragma once

#include "framework/parameters/input_parameters.h"

namespace opensn
{
class App;
}

namespace chi_physics
{

class FieldFunction;

/**Interface class to add a dependency on a logical volume. Two things need to
 * be done to use this interface. 1) Derive from it. 2) Add its parameters to
 * the child class. Now it will require a handle to a FieldFunction in
 * the input language.*/
class FieldFunctionInterface
{
protected:
  static chi::InputParameters GetInputParameters();

  explicit FieldFunctionInterface(opensn::App& app, const chi::InputParameters& params);

  std::shared_ptr<FieldFunction> GetFieldFunction() const;

  opensn::App& App() const { return app_; }

private:
  opensn::App& app_;
  chi::ParameterBlock field_function_param_;
};

} // namespace chi_physics
