#pragma once

#include "framework/physics/field_function/field_function_interface.h"

namespace chi_physics
{

class FieldFunctionGridBased;

/**Interface class to add a dependency on a logical volume. Two things need to
 * be done to use this interface. 1) Derive from it. 2) Add its parameters to
 * the child class. Now it will require a handle to a GridBasedFieldFunction in
 * the input language.*/
class GridBasedFieldFunctionInterface : public FieldFunctionInterface
{
public:
  static chi::InputParameters GetInputParameters();

  explicit GridBasedFieldFunctionInterface(opensn::App& app, const chi::InputParameters& params);

  std::shared_ptr<FieldFunctionGridBased> GetGridBasedFieldFunction() const;
};

} // namespace chi_physics
