#include "gmock/gmock.h"
#include "framework/math/dynamic_matrix.h"

using namespace opensn;

TEST(DynamicMatrix, CtorNMVal)
{
  DynamicMatrix<double> mat(5, 7, 1.0);
  for (int i = 0; i < 5; i++)
    for (int j = 0; j < 7; j++)
      EXPECT_DOUBLE_EQ(mat[i][j], 1.);
}
