#include "framework/mesh/sweep_utilities/sweep_boundary/sweep_boundary.h"
#include "framework/app.h"
#include "framework/logging/log.h"
#include "framework/mpi/mpi.h"

namespace chi_mesh::sweep_management
{

double*
SweepBoundary::HeterogeneousPsiIncoming(uint64_t cell_local_id,
                                        unsigned int face_num,
                                        unsigned int fi,
                                        unsigned int angle_num,
                                        int group_num,
                                        size_t gs_ss_begin)
{
  App().Log().LogAllError()
    << "HeterogeneousPsiIncoming call made to boundary that has no such information.";
  opensn::App::Exit(EXIT_FAILURE);
  return nullptr;
}

double*
SweepBoundary::HeterogeneousPsiOutgoing(uint64_t cell_local_id,
                                        unsigned int face_num,
                                        unsigned int fi,
                                        unsigned int angle_num,
                                        size_t gs_ss_begin)
{
  App().Log().LogAllError()
    << "HeterogeneousPsiOutgoing call made to boundary that has no such information.";
  opensn::App::Exit(EXIT_FAILURE);
  return nullptr;
}

} // namespace chi_mesh::sweep_management
