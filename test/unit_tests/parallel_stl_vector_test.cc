#include "gmock/gmock.h"
#include "framework/runtime.h"
#include "framework/math/parallel_vector/parallel_stl_vector.h"

using namespace opensn;
using namespace testing;

namespace
{

ParallelSTLVector
InitializeVector()
{
  ParallelSTLVector vec(5, 10, opensn::mpi_comm);
  if (opensn::mpi_comm.rank() == 0) vec.SetValue(5, 2.0, VecOpType::SET_VALUE);
  else
    vec.SetValue(0, 1.0, VecOpType::SET_VALUE);
  vec.Assemble();
  return vec;
}

} // namespace

TEST(ParallelSTLVector, Assemble)
{
  if (opensn::mpi_comm.size() != 2) return;

  auto vec = InitializeVector();

  if (mpi_comm.rank() == 0) EXPECT_THAT(vec.LocalSTLData(), ElementsAre(1, 0, 0, 0, 0));
  else if (mpi_comm.rank() == 1)
    EXPECT_THAT(vec.LocalSTLData(), ElementsAre(2, 0, 0, 0, 0));
}

TEST(ParallelSTLVector, Norms)
{
  if (opensn::mpi_comm.size() != 2) return;

  auto vec = InitializeVector();

  EXPECT_NEAR(vec.ComputeNorm(NormType::L1_NORM), 3, 1e-15);
  EXPECT_NEAR(vec.ComputeNorm(NormType::L2_NORM), 2.23607, 1e-5);
  EXPECT_NEAR(vec.ComputeNorm(NormType::LINF_NORM), 2, 1e-15);
}

TEST(ParallelSTLVector, Copy)
{
  if (opensn::mpi_comm.size() != 2) return;

  auto vec = InitializeVector();
  ParallelSTLVector vec2(5, 10, opensn::mpi_comm);
  vec2.CopyLocalValues(vec);
  if (mpi_comm.rank() == 0) EXPECT_THAT(vec2.LocalSTLData(), ElementsAre(1, 0, 0, 0, 0));
  else if (mpi_comm.rank() == 1)
    EXPECT_THAT(vec2.LocalSTLData(), ElementsAre(2, 0, 0, 0, 0));
}

TEST(ParallelSTLVector, AddValues)
{
  if (opensn::mpi_comm.size() != 2) return;

  auto vec = InitializeVector();
  if (opensn::mpi_comm.rank() == 0) vec.SetValue(5, 2.0, VecOpType::ADD_VALUE);
  else
    vec.SetValue(0, 1.0, VecOpType::ADD_VALUE);
  vec.Assemble();
  if (mpi_comm.rank() == 0) EXPECT_THAT(vec.LocalSTLData(), ElementsAre(2, 0, 0, 0, 0));
  else if (mpi_comm.rank() == 1)
    EXPECT_THAT(vec.LocalSTLData(), ElementsAre(4, 0, 0, 0, 0));
}

TEST(ParallelSTLVector, SetValues)
{
  if (opensn::mpi_comm.size() != 2) return;

  ParallelSTLVector vec(5, 10, opensn::mpi_comm);

  if (opensn::mpi_comm.rank() == 0) vec.SetValues({5, 6}, {2.0, 3.0}, VecOpType::ADD_VALUE);
  else
    vec.SetValues({0, 1}, {1.0, 4.0}, VecOpType::ADD_VALUE);
  vec.Assemble();
}
