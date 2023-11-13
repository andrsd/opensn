#pragma once

#include "framework/object.h"
#include "framework/parameters/input_parameters.h"

namespace chi_unit_testsB
{

class TestSubObject : public chi::ChiObject
{
private:
  const size_t num_groups_;

public:
  static chi::InputParameters GetInputParameters();
  explicit TestSubObject(opensn::App& app, const chi::InputParameters& params);
};

class TestObject : public chi::ChiObject
{
private:
  const std::string solver_type_;
  TestSubObject sub_obj1_;
  TestSubObject sub_obj2_;

public:
  static chi::InputParameters GetInputParameters();
  //  explicit TestObject(opensn::App& app, const chi::InputParameters& params);
};

class ChildTestObject : public TestObject
{
private:
  const int num_sub_groups_;

public:
  static chi::InputParameters GetInputParameters();
  explicit ChildTestObject(opensn::App& app, const chi::InputParameters& params);
};

} // namespace chi_unit_testsB
