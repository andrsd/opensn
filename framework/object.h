#pragma once

#include "framework/runtime.h"
#include "framework/parameters/input_parameters.h"

namespace opensn
{

class Object
{
public:
  /**Returns the input parameters. For the base Object, there
   * are now parameters loaded.*/
  static InputParameters GetInputParameters();

  /**Default constructor. This will be removed in future.*/
  Object();

  /**Constructor with input parameters.*/
  explicit Object(const InputParameters& params);
  
  virtual ~Object() = default;
};

} // namespace opensn
