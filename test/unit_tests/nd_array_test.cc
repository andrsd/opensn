#include "gmock/gmock.h"
#include "framework/data_types/ndarray.h"

using namespace opensn;
using namespace testing;

TEST(nd_array, ctor)
{
  NDArray<double> nd_array(std::vector<size_t>{2, 2, 2});
  std::vector<double> vals(nd_array.begin(), nd_array.end());
  EXPECT_THAT(vals, ElementsAre(0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(nd_array, ctor_array)
{
  NDArray<double> nd_array(std::array<size_t, 3>{2, 2, 2});
  std::vector<double> vals(nd_array.begin(), nd_array.end());
  EXPECT_THAT(vals, ElementsAre(0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(nd_array, ctor_init_list)
{
  NDArray<double> nd_array({2, 2, 2});
  std::vector<double> vals(nd_array.begin(), nd_array.end());
  EXPECT_THAT(vals, ElementsAre(0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(nd_array, ctor_std_vector)
{
  NDArray<double> nd_array(std::vector<size_t>{2, 2, 2}, 0.0);
  std::vector<double> vals(nd_array.begin(), nd_array.end());
  EXPECT_THAT(vals, ElementsAre(0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(nd_array, ctor_std_array)
{
  NDArray<double> nd_array(std::array<size_t, 3>{2, 2, 2}, 0.0);
  std::vector<double> vals(nd_array.begin(), nd_array.end());
  EXPECT_THAT(vals, ElementsAre(0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(ndarray, ctor_list)
{
  NDArray<double> nd_array({2, 2, 2}, 0.0);
  std::vector<double> vals(nd_array.begin(), nd_array.end());
  EXPECT_THAT(vals, ElementsAre(0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(nd_array, ctor_none)
{
  NDArray<double> nd_array;
  std::vector<double> vals(nd_array.begin(), nd_array.end());
  EXPECT_THAT(vals, ElementsAre());
}

TEST(nd_array, iter_const)
{
  NDArray<double> nd_array(std::vector<size_t>{2, 2, 2});
  nd_array.set(1.0);
  std::vector<double> vals(nd_array.begin(), nd_array.end());
  EXPECT_THAT(vals, ElementsAre(1., 1., 1., 1., 1., 1., 1., 1.));
}

TEST(nd_array, size)
{
  NDArray<double> nd_array(std::array<size_t, 3>{2, 2, 2});
  nd_array.set(1.0);
  EXPECT_EQ(nd_array.size(), 8);
}

TEST(nd_array, empty)
{
  NDArray<double> nd_array1(std::array<size_t, 3>{2, 2, 2});
  EXPECT_FALSE(nd_array1.empty());

  NDArray<double> nd_array2;
  EXPECT_TRUE(nd_array2.empty());
}

TEST(nd_array, dimension)
{
  NDArray<double> nd_array(std::array<size_t, 3>{2, 4, 7});
  nd_array.set(1.0);
  EXPECT_THAT(nd_array.dimension(), ElementsAre(2, 4, 7));
}

TEST(nd_array, rank)
{
  NDArray<double> nd_array(std::array<size_t, 3>{2, 4, 7});
  EXPECT_EQ(nd_array.rank(), 3);
}
