#include "gmock/gmock.h"
#include "framework/parameters/parameter_block.h"

using namespace opensn;
using namespace testing;

namespace
{

ParameterBlock
InitializeBlock()
{
  ParameterBlock param_block("block");
  param_block.AddParameter("enabled", true);
  param_block.AddParameter("it_method", "gmres");
  param_block.AddParameter("nl_abs_tol", 1.0e-12);
  param_block.AddParameter("nl_max_its", 33);
  return param_block;
}

} // namespace

TEST(ParameterBlock, CopyCtor)
{
  ParameterBlock blk = InitializeBlock();
  const auto& copy = blk;
  EXPECT_TRUE(copy.GetParamValue<bool>("enabled"));
  EXPECT_EQ(copy.GetParamValue<std::string>("it_method"), "gmres");
  EXPECT_DOUBLE_EQ(copy.GetParamValue<double>("nl_abs_tol"), 1.e-12);
  EXPECT_EQ(copy.GetParamValue<int>("nl_max_its"), 33);
}

TEST(ParameterBlock, MoveCtor)
{
  ParameterBlock blk = InitializeBlock();
  const auto& copy = blk;
  EXPECT_TRUE(copy.GetParamValue<bool>("enabled"));
  EXPECT_EQ(copy.GetParamValue<std::string>("it_method"), "gmres");
  EXPECT_DOUBLE_EQ(copy.GetParamValue<double>("nl_abs_tol"), 1.e-12);
  EXPECT_EQ(copy.GetParamValue<int>("nl_max_its"), 33);
}

TEST(ParameterBlock, RecursiveDumpToString)
{
  ParameterBlock blk = InitializeBlock();
  std::string str;
  blk.RecursiveDumpToString(str);
  EXPECT_THAT(str,
              StrEq("block = \n{\n  enabled = true,\n  it_method = \"gmres\",\n  nl_abs_tol = "
                    "0.000000,\n  nl_max_its = 33,\n}\n"));
}
