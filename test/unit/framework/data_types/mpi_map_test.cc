#include "framework/data_types/byte_array.h"
#include "framework/runtime.h"
#include "mpicpp-lite/impl/Communicator.h"
#include "test/unit/opensn_unit_test.h"
#include <gtest/gtest.h>

using namespace opensn;

class MpiMapTest : public OpenSnUnitTest
{
};

namespace
{

std::map<int, std::string>
mpi_allgather_map(const std::map<int, std::string>& local, mpi::Communicator comm)
{
  auto rank = comm.rank();
  auto nprocs = comm.size();

  // serialize local map
  ByteArray sendbuf;
  sendbuf.Write<std::size_t>(local.size());
  for (const auto& [k, v] : local)
  {
    sendbuf.Write<int>(k);
    sendbuf.Write<std::string>(v);
  }
  auto sendcount = static_cast<int>(sendbuf.Size());

  // gather sizes
  std::vector<int> recvcounts(nprocs);
  MPI_Allgather(&sendcount, 1, MPI_INT, recvcounts.data(), 1, MPI_INT, comm);

  // compute displacements
  std::vector<int> displs(nprocs, 0);
  for (int i = 1; i < nprocs; ++i)
    displs[i] = displs[i - 1] + recvcounts[i - 1];

  // total buffer
  int total = displs.back() + recvcounts.back();
  std::vector<std::byte> recvbuf(total);

  // gather actual data
  // FIXME: use mpi_comm.all_gather();
  MPI_Allgatherv(sendbuf.Data().data(),
                 sendcount,
                 MPI_BYTE,
                 recvbuf.data(),
                 recvcounts.data(),
                 displs.data(),
                 MPI_BYTE,
                 comm);

  // merge all maps
  std::map<int, std::string> merged;
  for (int i = 0; i < nprocs; ++i)
  {
    std::vector<std::byte> chunk(recvbuf.data() + displs[i],
                                 recvbuf.data() + displs[i] + recvcounts[i]);
    // auto m = deserialize_map(chunk);
    // merged.insert(m.begin(), m.end());
  }

  return merged;
}

} // namespace

TEST_F(MpiMapTest, WriteRead)
{
  auto mpi_comm = opensn::mpi_comm;
  auto rank = mpi_comm.rank();

  std::map<int, std::string> local = {{rank, "rank_" + std::to_string(rank)}};

  auto global = mpi_allgather_map(local, mpi_comm);

  if (rank == 0)
  {
    for (auto& [k, v] : global)
      std::cout << k << " => " << v << '\n';
  }

  // std::size_t size = 0;
  // for (auto& [k, v] : local)
  // {
  //   size += sizeof(k);
  //   size += sizeof(std::size_t) + v.size();
  // }

  // ByteArray serialized(size);
  // for (auto& [k, v] : local)
  // {
  //   serialized.Write<int>(k);
  //   serialized.Write<std::string>(v);
  // }

  // serialized.Seek(0);

  // auto m = serialized.Read<int>();
  // auto w = serialized.Read<std::string>();

  // EXPECT_
}
