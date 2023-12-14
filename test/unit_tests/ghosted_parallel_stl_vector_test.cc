#include "gmock/gmock.h"
#include "framework/runtime.h"
#include "framework/math/parallel_vector/ghosted_parallel_stl_vector.h"

using namespace opensn;
using namespace testing;

namespace
{

GhostedParallelSTLVector
InitializeVector()
{
  const int64_t ghost_id = opensn::mpi_comm.rank() == 0 ? 5 : 4;
  GhostedParallelSTLVector ghost_vec(5, 10, {ghost_id}, opensn::mpi_comm);

  if (opensn::mpi_comm.rank() == 0) ghost_vec.SetValue(5, 2.0, VecOpType::SET_VALUE);
  else
    ghost_vec.SetValue(4, 1.0, VecOpType::SET_VALUE);
  return ghost_vec;
}

} // namespace

TEST(GhostedParallelSTLVector, Assemble)
{
  if (opensn::mpi_comm.size() != 2) return;

  auto ghost_vec = InitializeVector();
  EXPECT_EQ(ghost_vec.NumGhosts(), 1);
  ghost_vec.Assemble();
  ghost_vec.CommunicateGhostEntries();
  if (opensn::mpi_comm.rank() == 0)
    EXPECT_THAT(ghost_vec.LocalSTLData(), ElementsAre(0, 0, 0, 0, 1, 2));
  else
    EXPECT_THAT(ghost_vec.LocalSTLData(), ElementsAre(2, 0, 0, 0, 0, 1));

  const auto made_vals = ghost_vec.MakeLocalVector();
  if (opensn::mpi_comm.rank() == 0) EXPECT_THAT(made_vals, ElementsAre(0, 0, 0, 0, 1));
  else
    EXPECT_THAT(made_vals, ElementsAre(2, 0, 0, 0, 0));
}

TEST(GhostedParallelSTLVector, Norms)
{
  if (opensn::mpi_comm.size() != 2) return;

  auto ghost_vec = InitializeVector();
  ghost_vec.Assemble();
  ghost_vec.CommunicateGhostEntries();

  EXPECT_NEAR(ghost_vec.ComputeNorm(NormType::L1_NORM), 3, 1e-15);
  EXPECT_NEAR(ghost_vec.ComputeNorm(NormType::L2_NORM), 2.23607, 1e-5);
  EXPECT_NEAR(ghost_vec.ComputeNorm(NormType::LINF_NORM), 2, 1e-15);
}

TEST(GhostedParallelSTLVector, FromComm)
{
  if (opensn::mpi_comm.size() != 2) return;

  auto ghost_vec = InitializeVector();
  ghost_vec.Assemble();
  ghost_vec.CommunicateGhostEntries();

  std::vector<int64_t> ghost_ids;
  if (opensn::mpi_comm.rank() == 0) ghost_ids = {5, 6};
  else
    ghost_ids = {0, 1, 3};
  VectorGhostCommunicator vgc(5, 10, ghost_ids, opensn::mpi_comm);
  GhostedParallelSTLVector ghost_vec2(vgc);
  if (opensn::mpi_comm.rank() == 0) ghost_vec2.SetValues({5, 6}, {6.0, 7.0}, VecOpType::ADD_VALUE);
  else
    ghost_vec2.SetValues({0, 1, 3}, {1.0, 2.0, 4.0}, VecOpType::ADD_VALUE);
  ghost_vec2.Assemble();
  ghost_vec2.CommunicateGhostEntries();

  if (opensn::mpi_comm.rank() == 0)
  {
    EXPECT_EQ(ghost_vec2.LocalSizeWithGhosts(), 7);
    EXPECT_THAT(ghost_vec2.LocalSTLData(), ElementsAre(1, 2, 0, 4, 0, 6, 7));
  }
  else
  {
    EXPECT_EQ(ghost_vec2.LocalSizeWithGhosts(), 8);
    EXPECT_THAT(ghost_vec2.LocalSTLData(), ElementsAre(6, 7, 0, 0, 0, 1, 2, 4));
  }
}

TEST(GhostedParallelSTLVector, SetValues)
{
  if (opensn::mpi_comm.size() != 2) return;

  auto ghost_vec = InitializeVector();
  ghost_vec.Assemble();
  ghost_vec.CommunicateGhostEntries();

  std::vector<int64_t> ghost_ids;
  if (opensn::mpi_comm.rank() == 0) ghost_ids = {5, 6};
  else
    ghost_ids = {0, 1, 3};
  VectorGhostCommunicator vgc(5, 10, ghost_ids, opensn::mpi_comm);
  GhostedParallelSTLVector ghost_vec2(vgc);
  if (opensn::mpi_comm.rank() == 0) ghost_vec2.SetValues({5, 6}, {6.0, 7.0}, VecOpType::ADD_VALUE);
  else
    ghost_vec2.SetValues({0, 1, 3}, {1.0, 2.0, 4.0}, VecOpType::ADD_VALUE);
  ghost_vec2.Assemble();
  ghost_vec2.CommunicateGhostEntries();

  const auto ghosted_local = ghost_vec2.MakeGhostedLocalVector();
  if (opensn::mpi_comm.rank() == 0)
  {
    EXPECT_THAT(ghost_vec2.GhostIndices(), ElementsAre(5, 6));
    EXPECT_EQ(ghost_vec2.MapGhostToLocal(6), 6);
    EXPECT_THAT(ghost_vec2.LocalSTLData(), ElementsAre(1, 2, 0, 4, 0, 6, 7));
    EXPECT_THAT(ghosted_local, ElementsAre(1, 2, 0, 4, 0, 6, 7));
    EXPECT_DOUBLE_EQ(ghost_vec2.GetGlobalValue(3), 4.);
    EXPECT_DOUBLE_EQ(ghost_vec2.GetGlobalValue(6), 7.);
  }
  else
  {
    EXPECT_THAT(ghost_vec2.GhostIndices(), ElementsAre(0, 1, 3));
    EXPECT_EQ(ghost_vec2.MapGhostToLocal(1), 6);
    EXPECT_THAT(ghost_vec2.LocalSTLData(), ElementsAre(6, 7, 0, 0, 0, 1, 2, 4));
    EXPECT_THAT(ghosted_local, ElementsAre(6, 7, 0, 0, 0, 1, 2, 4));
    EXPECT_DOUBLE_EQ(ghost_vec2.GetGlobalValue(6), 7.);
    EXPECT_DOUBLE_EQ(ghost_vec2.GetGlobalValue(1), 2.);
  }
}
