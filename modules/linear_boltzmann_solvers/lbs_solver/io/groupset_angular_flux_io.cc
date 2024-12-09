// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#include "modules/linear_boltzmann_solvers/lbs_solver/io/lbs_solver_io.h"
#include "modules/linear_boltzmann_solvers/lbs_solver/lbs_solver.h"
#include "framework/mesh/mesh_continuum/mesh_continuum.h"

namespace opensn
{

void
LBSSolverIO::WriteGroupsetAngularFluxes(
  LBSSolver& lbs_solver,
  const int groupset_id,
  const std::string& file_base,
  std::optional<const std::reference_wrapper<std::vector<double>>> opt_src)
{
  assert(groupset_id >= 0 and groupset_id < lbs_solver.Groupsets().size());

  // Open file
  const auto file_name = file_base + std::to_string(opensn::mpi_comm.rank()) + ".data";
  std::ofstream file(file_name, std::ofstream::binary | std::ofstream::out | std::ofstream::trunc);
  OpenSnLogicalErrorIf(not file.is_open(), "Failed to open " + file_name + ".");

  auto& groupset = lbs_solver.GetGroupsets().at(groupset_id);
  std::vector<double>& src =
    opt_src.has_value() ? opt_src.value().get() : lbs_solver.GetPsiNewLocal().at(groupset_id);

  log.Log() << "Writing groupset " << groupset_id << " angular flux file to " << file_base;

  // Write header
  const int num_bytes = 320;
  std::string header_info = "OpenSn LinearBoltzmannSolver::Groupset angular flux file\n"
                            "Header size: " +
                            std::to_string(num_bytes) +
                            " bytes\n"
                            "Structure(type-info):\n"
                            "uint64_t   num_local_nodes\n"
                            "uint64_t   num_angles\n"
                            "uint64_t   num_groups\n"
                            "Each record:\n"
                            "  uint64_t   cell_global_id\n"
                            "  uint64_t   node\n"
                            "  uint64_t   angle\n"
                            "  uint64_t   group\n"
                            "  double     value\n";

  int header_size = (int)header_info.length();
  char header_bytes[num_bytes];
  memset(header_bytes, '-', num_bytes);
  strncpy(header_bytes, header_info.c_str(), std::min(header_size, num_bytes - 1));
  header_bytes[num_bytes - 1] = '\0';

  file << header_bytes;

  // Write macro info
  const auto& uk_man = groupset.psi_uk_man_;
  const auto NODES_ONLY = UnknownManager::GetUnitaryUnknownManager();
  auto& discretization = lbs_solver.GetSpatialDiscretization();
  auto& grid = lbs_solver.GetGrid();
  const uint64_t num_local_nodes = discretization.GetNumLocalDOFs(NODES_ONLY);
  const uint64_t num_gs_angles = groupset.quadrature->abscissae.size();
  const uint64_t num_gs_groups = groupset.groups.size();
  const auto num_local_gs_dofs = discretization.GetNumLocalDOFs(uk_man);

  OpenSnLogicalErrorIf(src.size() != num_local_gs_dofs,
                       "Incompatible angular flux vector provided for groupset " +
                         std::to_string(groupset.id) + ".");

  file.write((char*)&num_local_nodes, sizeof(uint64_t));
  file.write((char*)&num_gs_angles, sizeof(uint64_t));
  file.write((char*)&num_gs_groups, sizeof(uint64_t));

  // Write the groupset angular flux data
  for (const auto& cell : grid.local_cells)
    for (uint64_t i = 0; i < discretization.GetCellNumNodes(cell); ++i)
      for (uint64_t n = 0; n < num_gs_angles; ++n)
        for (uint64_t g = 0; g < num_gs_groups; ++g)
        {
          const uint64_t dof_map = discretization.MapDOFLocal(cell, i, uk_man, n, g);
          const double value = src[dof_map];

          file.write((char*)&cell.global_id, sizeof(uint64_t));
          file.write((char*)&i, sizeof(uint64_t));
          file.write((char*)&n, sizeof(uint64_t));
          file.write((char*)&g, sizeof(uint64_t));
          file.write((char*)&value, sizeof(double));
        }
  file.close();
}

void
LBSSolverIO::ReadGroupsetAngularFluxes(
  LBSSolver& lbs_solver,
  const int groupset_id,
  const std::string& file_base,
  std::optional<std::reference_wrapper<std::vector<double>>> opt_dest)

{
  assert(groupset_id >= 0 and groupset_id < lbs_solver.Groupsets().size());

  // Open file
  const auto file_name = file_base + std::to_string(opensn::mpi_comm.rank()) + ".data";
  std::ifstream file(file_name, std::ofstream::binary | std::ofstream::in);
  OpenSnLogicalErrorIf(not file.is_open(), "Failed to open " + file_name + ".");

  auto& groupsets = lbs_solver.GetGroupsets();
  auto& groupset = groupsets.at(groupset_id);
  std::vector<double>& dest =
    opt_dest.has_value() ? opt_dest.value().get() : lbs_solver.GetPsiNewLocal().at(groupset_id);

  log.Log() << "Reading groupset " << groupset.id << " angular flux file " << file_base;

  // Read the header
  const int num_bytes = 320;
  char header_bytes[num_bytes];
  header_bytes[num_bytes - 1] = '\0';
  file.read(header_bytes, num_bytes - 1);

  // Read the macro info
  uint64_t file_num_local_nodes;
  uint64_t file_num_gs_angles;
  uint64_t file_num_gs_groups;

  file.read((char*)&file_num_local_nodes, sizeof(uint64_t));
  file.read((char*)&file_num_gs_angles, sizeof(uint64_t));
  file.read((char*)&file_num_gs_groups, sizeof(uint64_t));

  // Check compatibility with system macro info
  const auto& uk_man = groupset.psi_uk_man_;
  const auto NODES_ONLY = UnknownManager::GetUnitaryUnknownManager();

  auto& discretization = lbs_solver.GetSpatialDiscretization();
  auto& grid = lbs_solver.GetGrid();
  const uint64_t num_local_nodes = discretization.GetNumLocalDOFs(NODES_ONLY);
  const uint64_t num_gs_angles = groupset.quadrature->abscissae.size();
  const uint64_t num_gs_groups = groupset.groups.size();
  const auto num_local_gs_dofs = discretization.GetNumLocalDOFs(uk_man);

  OpenSnLogicalErrorIf(file_num_local_nodes != num_local_nodes,
                       "Incompatible number of local nodes found in file " + file_name + ".");
  OpenSnLogicalErrorIf(file_num_gs_angles != num_gs_angles,
                       "Incompatible number of groupset angles found in file " + file_name +
                         " for groupset " + std::to_string(groupset.id) + ".");
  OpenSnLogicalErrorIf(file_num_gs_groups != num_gs_groups,
                       "Incompatible number of groupset groups found in file " + file_name +
                         " for groupset " + std::to_string(groupset.id) + ".");

  // Read the angular flux data
  dest.assign(num_local_gs_dofs, 0.0);
  for (uint64_t dof = 0; dof < num_local_gs_dofs; ++dof)
  {
    uint64_t cell_global_id;
    uint64_t node;
    uint64_t angle;
    uint64_t group;
    double psi_value;

    file.read((char*)&cell_global_id, sizeof(uint64_t));
    file.read((char*)&node, sizeof(uint64_t));
    file.read((char*)&angle, sizeof(uint64_t));
    file.read((char*)&group, sizeof(uint64_t));
    file.read((char*)&psi_value, sizeof(double));

    const auto& cell = grid.cells[cell_global_id];
    const auto imap = discretization.MapDOFLocal(cell, node, uk_man, angle, group);
    dest[imap] = psi_value;
  }
  file.close();
}

} // namespace opensn
