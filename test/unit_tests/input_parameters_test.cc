#include "gmock/gmock.h"
#include "framework/parameters/input_parameters.h"
#include "framework/object_factory.h"
#include "framework/runtime.h"
#include "framework/logging/log.h"

using namespace opensn;
using namespace testing;

namespace
{

class TestSubObject : public Object
{
private:
  const size_t num_groups_;

public:
  static InputParameters GetInputParameters();
  explicit TestSubObject(const InputParameters& params);
};

InputParameters
TestSubObject::GetInputParameters()
{
  InputParameters params;
  params.SetGeneralDescription("General test sub-object");
  params.AddRequiredParameter<size_t>("num_groups", "Number of groups to use in the simulation");
  return params;
}

OpenSnRegisterObjectNamespace(unit_tests, TestSubObject);

TestSubObject::TestSubObject(const InputParameters& params)
  : num_groups_(params.GetParamValue<size_t>("num_groups"))
{
  opensn::log.Log() << "TestSubObject created "
                    << "num_groups=" << num_groups_;
}

// --

class TestObject : public Object
{
private:
  const std::string solver_type_;
  TestSubObject sub_obj1_;
  TestSubObject sub_obj2_;

public:
  static InputParameters GetInputParameters();
  explicit TestObject(const InputParameters& params);
};

OpenSnRegisterObjectNamespace(unit_tests, TestObject);

InputParameters
TestObject::GetInputParameters()
{
  InputParameters params = Object::GetInputParameters();

  params.SetGeneralDescription("General test object");

  params.AddOptionalParameter("solver_type", "A", "The solver type.");
  params.AddRequiredParameter<std::string>("coupled_field", "The text name of the coupled field.");
  params.AddRequiredParameterBlock("sub_obj1",
                                   "A block of parameters for unit_testsB::TestSubObject");

  ParameterBlock sub_obj2_param_block("sub_obj2");
  sub_obj2_param_block.AddParameter("num_groups", 99);
  params.AddOptionalParameterBlock(
    "sub_obj2", sub_obj2_param_block, "A block of parameters for unit_testsB::TestSubObject");

  params.AddOptionalParameter("limiter_type", 1, "Type of limiter to use in the solver");
  params.MarkParamaterDeprecatedWarning("limiter_type");

  params.AddOptionalParameter("scheme", "Zorba", "What scheme to use");
  params.MarkParamaterDeprecatedError("scheme");

  params.AddRequiredParameter<bool>("format", "What output format to use");
  params.MarkParamaterDeprecatedError("format");

  params.AddOptionalParameter("use_my_stuff", false, "Yeah please do");
  params.MarkParamaterRenamed("use_my_stuff", "Renamed to \"use_zaks_stuff\".");

  params.AddRequiredParameter<bool>("use_ragusas_stuff", "If you want");
  params.MarkParamaterRenamed("use_ragusas_stuff", "Renamed to \"use_complicated_stuff\".");

  params.AddOptionalParameter<int>(
    "groupset_num_subsets",
    1,
    "The number of subsets to apply to the set of groups in this set. This is "
    "useful for increasing pipeline size for parallel simulations");

  params.ConstrainParameterRange("groupset_num_subsets", AllowableRangeLowLimit::New<int>(1));
  return params;
}

TestObject::TestObject(const InputParameters& params)
  : solver_type_(params.GetParamValue<std::string>("solver_type")),
    sub_obj1_(InputParameters::MakeForObject<TestSubObject>(params.GetParam("sub_obj1"))),
    sub_obj2_(InputParameters::MakeForObject<TestSubObject>(params.GetParam("sub_obj2")))
{
  opensn::log.Log() << "TestObject created "
                    << "solver_type=" << solver_type_;
}

// --

class ChildTestObject : public TestObject
{
private:
  const int num_sub_groups_;

public:
  static InputParameters GetInputParameters();
  explicit ChildTestObject(const InputParameters& params);
};

OpenSnRegisterObjectNamespace(unit_tests, ChildTestObject);

InputParameters
ChildTestObject::GetInputParameters()
{
  InputParameters params = TestObject::GetInputParameters();
  params.SetGeneralDescription("General test child-object inheriting option from parent");
  params.ChangeExistingParamToOptional("coupled_field", "Q");
  params.ChangeExistingParamToRequired<std::string>("solver_type");
  params.AddOptionalParameter("num_sub_groups", 1, "Number of sub-groups to use in the simulation");
  return params;
}

ChildTestObject::ChildTestObject(const InputParameters& params)
  : TestObject(params), num_sub_groups_(params.GetParamValue<int>("num_sub_groups"))
{
  opensn::log.Log() << "ChildTestObject created "
                    << "num_sub_groups=" << num_sub_groups_;
}

} // namespace

TEST(InputParameters, DeprecatedWarning)
{
  internal::CaptureStdout();

  ParameterBlock sub_blk("sub_obj1");
  sub_blk.AddParameter("num_groups", 2);

  ParameterBlock blk;
  blk.AddParameter("solver_type", "B");
  blk.AddParameter("coupled_field", "T");
  blk.AddParameter(sub_blk);
  blk.AddParameter("limiter_type", 2);

  InputParameters params = TestObject::GetInputParameters();
  params.AssignParameters(blk);

  auto out = internal::GetCapturedStdout();
  EXPECT_THAT(
    out,
    HasSubstr(
      "**WARNING** Parameter \"limiter_type\" has been deprecated and will be removed soon."));
}

TEST(InputParameters, DeprecatedErrorOptional)
{
  ParameterBlock sub_blk("sub_obj1");
  sub_blk.AddParameter("num_groups", 2);

  ParameterBlock blk;
  blk.AddParameter("solver_type", "B");
  blk.AddParameter("coupled_field", "T");
  blk.AddParameter(sub_blk);
  blk.AddParameter("scheme", "Snotty");

  InputParameters params = TestObject::GetInputParameters();
  EXPECT_DEATH(params.AssignParameters(blk), "Parameter \"scheme\" has been deprecated");
}

TEST(InputParameters, DeprecatedErrorRequired)
{
  ParameterBlock sub_blk("sub_obj1");
  sub_blk.AddParameter("num_groups", 2);

  ParameterBlock blk;
  blk.AddParameter("solver_type", "B");
  blk.AddParameter("coupled_field", "T");
  blk.AddParameter(sub_blk);
  blk.AddParameter("format", true);

  InputParameters params = TestObject::GetInputParameters();
  EXPECT_DEATH(params.AssignParameters(blk), "Parameter \"format\" has been deprecated");
}

TEST(InputParameters, TestChildObject)
{
  ParameterBlock sub_blk("sub_obj1");
  sub_blk.AddParameter("num_groups", 2);

  ParameterBlock blk;
  blk.AddParameter("solver_type", "C");
  blk.AddParameter("coupled_field", "T");
  blk.AddParameter(sub_blk);
  blk.AddParameter("num_sub_groups", 3);

  InputParameters params = ChildTestObject::GetInputParameters();
  params.AssignParameters(blk);
}

TEST(InputParameters, RenameParamOptional)
{
  ParameterBlock sub_blk("sub_obj1");
  sub_blk.AddParameter("num_groups", 2);

  ParameterBlock blk;
  blk.AddParameter("solver_type", "B");
  blk.AddParameter("coupled_field", "T");
  blk.AddParameter(sub_blk);
  blk.AddParameter("use_my_stuff", true);

  InputParameters params = TestObject::GetInputParameters();
  EXPECT_THROW(params.AssignParameters(blk), std::invalid_argument);
}

TEST(InputParameters, RenameParamRequired)
{
  ParameterBlock sub_blk("sub_obj1");
  sub_blk.AddParameter("num_groups", 2);

  ParameterBlock blk;
  blk.AddParameter("solver_type", "B");
  blk.AddParameter("coupled_field", "T");
  blk.AddParameter(sub_blk);
  blk.AddParameter("use_ragusas_stuff", true);

  InputParameters params = TestObject::GetInputParameters();
  EXPECT_THROW(params.AssignParameters(blk), std::invalid_argument);
}
