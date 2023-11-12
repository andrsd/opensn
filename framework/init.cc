#include "framework/init.h"

namespace opensn
{

Init::Init(int argc, char** argv)
{
  //  int location_id = 0, number_processes = 1;
  //  MPI_Comm communicator = MPI_COMM_WORLD;
  //
  //  MPI_Init(&argc, &argv);
  //  MPI_Comm_rank(communicator, &location_id);
  //  MPI_Comm_size(communicator, &number_processes);
  //
  //  Chi::mpi.SetCommunicator(communicator);
  //  Chi::mpi.SetLocationID(location_id);
  //  Chi::mpi.SetProcessCount(number_processes);
  //
  //  Chi::run_time::InitPetSc(argc, argv);
}

Init::~Init()
{
  //  Chi::Finalize();
}

} // namespace opensn
