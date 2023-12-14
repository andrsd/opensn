#include "gmock/gmock.h"
#include "framework/data_types/varying.h"

using namespace opensn;
using namespace testing;

TEST(varying, type_int)
{
  Varying v(12);
  EXPECT_EQ(v.IntegerValue(), 12);
  EXPECT_EQ(v.GetValue<int>(), 12);
  EXPECT_EQ(v.GetValue<std::size_t>(), 12);
}

TEST(varying, type_bool)
{
  Varying v(true);
  EXPECT_TRUE(v.BoolValue());
  EXPECT_TRUE(v.GetValue<bool>());
}

TEST(varying, type_float)
{
  Varying v(12.0);
  EXPECT_DOUBLE_EQ(v.FloatValue(), 12.);
  EXPECT_DOUBLE_EQ(v.GetValue<double>(), 12.);
  EXPECT_DOUBLE_EQ(v.GetValue<float>(), 12.);
  EXPECT_EQ(v.ByteSize(), 8);
}

TEST(varying, type_string)
{
  Varying v(std::string("Hello"));
  EXPECT_EQ(v.StringValue(), "Hello");
  EXPECT_EQ(v.GetValue<std::string>(), "Hello");
}
