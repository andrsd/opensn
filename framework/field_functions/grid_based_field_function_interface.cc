// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#include "framework/field_functions/grid_based_field_function_interface.h"
#include "framework/field_functions/field_function_grid_based.h"

namespace opensn
{

InputParameters
GridBasedFieldFunctionInterface::GetInputParameters()
{
  InputParameters params = FieldFunctionInterface::GetInputParameters();

  return params;
}

GridBasedFieldFunctionInterface::GridBasedFieldFunctionInterface(const InputParameters& params)
  : FieldFunctionInterface(params)
{
}

std::shared_ptr<FieldFunctionGridBased>
GridBasedFieldFunctionInterface::GetGridBasedFieldFunction() const
{
  auto ff_ptr = GetFieldFunction();

  auto grid_based_ff_ptr = std::dynamic_pointer_cast<FieldFunctionGridBased>(ff_ptr);

  return grid_based_ff_ptr;
}

} // namespace opensn
