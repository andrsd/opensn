// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#include "modules/linear_boltzmann_solvers/discrete_ordinates_solver/sweep/angle_set/cbc_angle_set.h"
#include "modules/linear_boltzmann_solvers/discrete_ordinates_solver/sweep/communicators/cbc_async_comm.h"
#include "modules/linear_boltzmann_solvers/discrete_ordinates_solver/sweep/spds/cbc_spds.h"
#include "modules/linear_boltzmann_solvers/discrete_ordinates_solver/sweep_chunks/sweep_chunk.h"
#include "framework/mesh/mesh_continuum/mesh_continuum.h"
#include "framework/math/math_range.h"
#include "framework/logging/log.h"
#include "framework/runtime.h"
#include "caliper/cali.h"

namespace opensn
{

CBC_AngleSet::CBC_AngleSet(size_t id,
                           size_t num_groups,
                           const SPDS& spds,
                           std::shared_ptr<FLUDS>& fluds,
                           const std::vector<size_t>& angle_indices,
                           std::map<uint64_t, std::shared_ptr<SweepBoundary>>& boundaries,
                           size_t group_subset,
                           const MPICommunicatorSet& comm_set)
  : AngleSet(id, num_groups, spds, fluds, angle_indices, boundaries, group_subset),
    cbc_spds_(dynamic_cast<const CBC_SPDS&>(spds_)),
    async_comm_(id, *fluds, comm_set)
{
}

AsynchronousCommunicator*
CBC_AngleSet::GetCommunicator()
{
  return static_cast<AsynchronousCommunicator*>(&async_comm_);
}

AngleSetStatus
CBC_AngleSet::AngleSetAdvance(SweepChunk& sweep_chunk, AngleSetStatus permission)
{
  CALI_CXX_MARK_SCOPE("CBC_AngleSet::AngleSetAdvance");

  if (executed_)
    return AngleSetStatus::FINISHED;

  if (current_task_list_.empty())
    current_task_list_ = cbc_spds_.TaskList();

  sweep_chunk.SetAngleSet(*this);

  auto tasks_who_received_data = async_comm_.ReceiveData();

  for (const uint64_t task_number : tasks_who_received_data)
    --current_task_list_[task_number].num_dependencies_;

  async_comm_.SendData();

  // Check if boundaries allow for execution
  for (auto& [bid, boundary] : boundaries_)
    if (not boundary->CheckAnglesReadyStatus(angles_, group_subset_))
      return AngleSetStatus::NOT_FINISHED;

  bool all_tasks_completed = true;
  bool a_task_executed = true;
  while (a_task_executed)
  {
    a_task_executed = false;
    for (auto& cell_task : current_task_list_)
    {
      if (not cell_task.completed_)
        all_tasks_completed = false;
      if (cell_task.num_dependencies_ == 0 and not cell_task.completed_)
      {
        sweep_chunk.SetCell(cell_task.cell_ptr_, *this);
        sweep_chunk.Sweep(*this);

        for (uint64_t local_task_num : cell_task.successors_)
          --current_task_list_[local_task_num].num_dependencies_;

        cell_task.completed_ = true;
        a_task_executed = true;
        async_comm_.SendData();
      }
    } // for cell_task
    async_comm_.SendData();
  }

  const bool all_messages_sent = async_comm_.SendData();

  if (all_tasks_completed and all_messages_sent)
  {
    // Update boundary readiness
    for (auto& [bid, boundary] : boundaries_)
      boundary->UpdateAnglesReadyStatus(angles_, group_subset_);
    executed_ = true;
    return AngleSetStatus::FINISHED;
  }

  return AngleSetStatus::NOT_FINISHED;
}

void
CBC_AngleSet::ResetSweepBuffers()
{
  current_task_list_.clear();
  async_comm_.Reset();
  fluds_->ClearLocalAndReceivePsi();
  executed_ = false;
}

const double*
CBC_AngleSet::PsiBoundary(uint64_t boundary_id,
                          unsigned int angle_num,
                          uint64_t cell_local_id,
                          unsigned int face_num,
                          unsigned int fi,
                          int g,
                          size_t gs_ss_begin,
                          bool surface_source_active)
{
  std::cerr << "a\n";
  if (boundaries_[boundary_id]->IsReflecting())
    return boundaries_[boundary_id]->PsiIncoming(
      cell_local_id, face_num, fi, angle_num, g, gs_ss_begin);

  std::cerr << "surface_source_active = " << surface_source_active << "\n";
  if (not surface_source_active)
    return boundaries_[boundary_id]->ZeroFlux(g);

  std::cerr << "blb\n";
  return boundaries_[boundary_id]->PsiIncoming(
    cell_local_id, face_num, fi, angle_num, g, gs_ss_begin);
}

double*
CBC_AngleSet::PsiReflected(uint64_t boundary_id,
                           unsigned int angle_num,
                           uint64_t cell_local_id,
                           unsigned int face_num,
                           unsigned int fi,
                           size_t gs_ss_begin)
{
  return boundaries_[boundary_id]->PsiOutgoing(cell_local_id, face_num, fi, angle_num, gs_ss_begin);
}

} // namespace opensn
