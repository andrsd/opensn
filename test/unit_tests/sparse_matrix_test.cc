#include "gmock/gmock.h"
#include "framework/math/sparse_matrix/math_sparse_matrix.h"

using namespace opensn;
using namespace testing;

namespace
{

SparseMatrix
InitMatrix()
{
  SparseMatrix mat(4, 4);
  mat.Insert(0, 0, 1.0);
  mat.Insert(0, 1, 1.1);
  mat.Insert(0, 2, 1.2);
  mat.Insert(0, 3, 1.3);
  mat.Insert(1, 0, 1.9);
  mat.Insert(1, 1, 2.0);
  mat.Insert(1, 2, 2.1);
  mat.Insert(2, 1, 2.9);
  mat.Insert(2, 2, 3.0);
  mat.Insert(2, 3, 3.1);
  mat.Insert(3, 2, 3.9);
  mat.Insert(3, 3, 4.0);
  return mat;
}

} // namespace

TEST(SparseMatrix, Row)
{
  auto m = InitMatrix();

  std::vector<std::size_t> row_idxs = {2, 2, 2};
  std::vector<std::size_t> col_idxs = {1, 2, 3};
  std::vector<double> vals = {2.9, 3., 3.1};
  std::size_t i = 0;
  for (const auto& entry : m.Row(2))
  {
    EXPECT_EQ(entry.row_index, row_idxs[i]);
    EXPECT_EQ(entry.column_index, col_idxs[i]);
    EXPECT_DOUBLE_EQ(entry.value, vals[i]);
    i++;
  }
}

TEST(SparseMatrix, OperTimesEqual)
{
  auto m = InitMatrix();

  for (const auto& [row_index, column_index, value] : m.Row(2))
    value *= 2;

  std::vector<double> vals = {5.8, 6., 6.2};
  std::size_t i = 0;
  for (const auto& entry : m.Row(2))
  {
    EXPECT_DOUBLE_EQ(entry.value, vals[i]);
    i++;
  }
}
