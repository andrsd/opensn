#pragma once

#include "framework/object.h"

namespace opensn
{

class TestObj : public Object
{
public:
  explicit TestObj(const InputParameters& params);

public:
  static InputParameters GetInputParameters();
  static std::shared_ptr<TestObj> Create(const ParameterBlock& params);
};

} // namespace opensn
