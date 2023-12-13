#include "gmock/gmock.h"
#include "framework/math/dynamic_vector.h"

using namespace opensn;

TEST(DynamicVector, CtorNVals)
{
  DynamicVector<double> vec(5, 1.0);
  for (int i = 0; i < 5; i++)
    EXPECT_DOUBLE_EQ(vec[i], 1.);
}
