#include "framework/mesh/sweep_utilities/spds/spds_adams_adams_hawkins.h"
#include "framework/mesh/mesh_continuum/mesh_continuum.h"
#include "framework/app.h"
#include "framework/logging/log.h"
#include "framework/graphs/directed_graph.h"
#include <algorithm>

namespace chi_mesh::sweep_management
{

SPDS_AdamsAdamsHawkins::SPDS_AdamsAdamsHawkins(const chi_mesh::Vector3& omega,
                                               const chi_mesh::MeshContinuum& grid,
                                               bool cycle_allowance_flag,
                                               bool verbose)
  : SPDS(omega, grid, verbose)
{
  App().Log().Log0Verbose1() << App().ProgramTimer().GetTimeString()
                             << " Building sweep ordering for Omega = " << omega.PrintS();

  size_t num_loc_cells = grid.local_cells.size();

  // Populate Cell Relationships
  App().Log().Log0Verbose1() << "Populating cell relationships";
  std::vector<std::set<std::pair<int, double>>> cell_successors(num_loc_cells);
  std::set<int> location_successors;
  std::set<int> location_dependencies;

  PopulateCellRelationships(omega, location_dependencies, location_successors, cell_successors);

  location_successors_.reserve(location_successors.size());
  location_dependencies_.reserve(location_dependencies.size());

  for (auto v : location_successors)
    location_successors_.push_back(v);

  for (auto v : location_dependencies)
    location_dependencies_.push_back(v);

  // Build graph
  chi::DirectedGraph local_DG(App());

  // Add vertex for each local cell
  for (int c = 0; c < num_loc_cells; ++c)
    local_DG.AddVertex();

  // Create graph edges
  for (int c = 0; c < num_loc_cells; c++)
    for (auto& successor : cell_successors[c])
      local_DG.AddEdge(c, successor.first, successor.second);

  // Remove local cycles if allowed
  if (verbose_) PrintedGhostedGraph();

  if (cycle_allowance_flag)
  {
    App().Log().Log0Verbose1() << App().ProgramTimer().GetTimeString()
                               << " Removing inter-cell cycles.";

    auto edges_to_remove = local_DG.RemoveCyclicDependencies();

    for (auto& edge_to_remove : edges_to_remove)
    {
      local_cyclic_dependencies_.emplace_back(edge_to_remove.first, edge_to_remove.second);
    }
  }

  // Generate topological sorting
  App().Log().Log0Verbose1() << App().ProgramTimer().GetTimeString()
                             << " Generating topological sorting for local sweep ordering";
  auto so_temp = local_DG.GenerateTopologicalSort();
  spls_.item_id.clear();
  for (auto v : so_temp)
    spls_.item_id.emplace_back(v);

  if (spls_.item_id.empty())
  {
    App().Log().LogAllError() << "Topological sorting for local sweep-ordering failed. "
                              << "Cyclic dependencies detected. Cycles need to be allowed"
                              << " by calling application.";
    opensn::App::Exit(EXIT_FAILURE);
  }

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Create Task
  //                                                        Dependency Graphs
  // All locations will gather other locations' dependencies
  // so that each location has the ability to build
  // the global task graph.

  App().Log().Log0Verbose1() << App().ProgramTimer().GetTimeString()
                             << " Communicating sweep dependencies.";

  // auto& global_dependencies = sweep_order->global_dependencies;
  std::vector<std::vector<int>> global_dependencies;
  global_dependencies.resize(App().ProcessCount());

  CommunicateLocationDependencies(App(), location_dependencies_, global_dependencies);

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Build task
  //                                                        dependency graph
  BuildTaskDependencyGraph(global_dependencies, cycle_allowance_flag);

  App().Barrier();

  App().Log().Log0Verbose1() << App().ProgramTimer().GetTimeString()
                             << " Done computing sweep ordering.\n\n";
}

void
chi_mesh::sweep_management::SPDS_AdamsAdamsHawkins::BuildTaskDependencyGraph(
  const std::vector<std::vector<int>>& global_dependencies, bool cycle_allowance_flag)
{

  std::vector<std::pair<int, int>> edges_to_remove;
  std::vector<int> raw_edges_to_remove;
  chi::DirectedGraph TDG(App());

  // Build graph on home location
  if (App().LocationID() == 0)
  {
    App().Log().Log0Verbose1() << App().ProgramTimer().GetTimeString()
                               << " Building Task Dependency Graphs.";

    // Add vertices to the graph
    for (int loc = 0; loc < App().ProcessCount(); loc++)
      TDG.AddVertex();

    // Add dependencies
    for (int loc = 0; loc < App().ProcessCount(); loc++)
      for (int dep = 0; dep < global_dependencies[loc].size(); dep++)
        TDG.AddEdge(global_dependencies[loc][dep], loc);

    // Remove cyclic dependencies
    if (cycle_allowance_flag)
    {
      App().Log().Log0Verbose1() << App().ProgramTimer().GetTimeString()
                                 << " Removing intra-cellset cycles.";
      auto edges_to_remove_temp = TDG.RemoveCyclicDependencies();
      for (const auto& [v0, v1] : edges_to_remove_temp)
        edges_to_remove.emplace_back(v0, v1);
    }

    // Serialize edges to be removed
    raw_edges_to_remove.resize(edges_to_remove.size() * 2, 0);
    int i = 0;
    for (const auto& edge : edges_to_remove)
    {
      raw_edges_to_remove[i++] = edge.first;
      raw_edges_to_remove[i++] = edge.second;
    }
  } // if home

  // Broadcast edge buffer size
  int edge_buffer_size = 0;

  if (App().LocationID() == 0) edge_buffer_size = static_cast<int>(raw_edges_to_remove.size());

  MPI_Bcast(&edge_buffer_size, 1, MPI_INT, 0, App().Comm());

  // Broadcast edges
  if (App().LocationID() != 0) raw_edges_to_remove.resize(edge_buffer_size, -1);

  MPI_Bcast(raw_edges_to_remove.data(), edge_buffer_size, MPI_INT, 0, App().Comm());

  // De-serialize edges
  if (App().LocationID() != 0)
  {
    edges_to_remove.resize(edge_buffer_size / 2, std::pair<int, int>(0, 0));
    int i = 0;
    for (auto& edge : edges_to_remove)
    {
      edge.first = raw_edges_to_remove[i++];
      edge.second = raw_edges_to_remove[i++];
    }
  }

  // Remove edges
  for (auto& edge_to_remove : edges_to_remove)
  {
    int rlocI = edge_to_remove.first;
    int locI = edge_to_remove.second;

    if (App().LocationID() == 0) TDG.RemoveEdge(rlocI, locI);

    if (locI == App().LocationID())
    {
      auto dependent_location =
        std::find(location_dependencies_.begin(), location_dependencies_.end(), rlocI);
      location_dependencies_.erase(dependent_location);
      delayed_location_dependencies_.push_back(rlocI);
    }

    if (rlocI == App().LocationID()) delayed_location_successors_.push_back(locI);
  }

  // Generate topological sort
  std::vector<int> glob_linear_sweep_order;
  if (App().LocationID() == 0)
  {
    App().Log().LogAllVerbose2() << App().ProgramTimer().GetTimeString()
                                 << "   - Generating topological sort.";
    auto so_temp = TDG.GenerateTopologicalSort();
    for (auto v : so_temp)
      glob_linear_sweep_order.emplace_back(v);

    if (glob_linear_sweep_order.empty())
    {
      App().Log().LogAllError() << "Topological sorting for global sweep-ordering failed. "
                                << "Cyclic dependencies detected. Cycles need to be allowed"
                                << " by calling application.";
      opensn::App::Exit(EXIT_FAILURE);
    }
  }

  // Broadcasting topsort size
  int topsort_buffer_size = 0;

  if (App().LocationID() == 0) topsort_buffer_size = glob_linear_sweep_order.size();

  MPI_Bcast(&topsort_buffer_size, 1, MPI_INT, 0, App().Comm());

  // Broadcast topological sort
  if (App().LocationID() != 0) glob_linear_sweep_order.resize(topsort_buffer_size, -1);

  MPI_Bcast(glob_linear_sweep_order.data(), topsort_buffer_size, MPI_INT, 0, App().Comm());

  // Compute reorder mapping
  // This mapping allows us to punch in
  // the location id and find what its
  // id is in the TDG
  std::vector<int> glob_order_mapping(App().ProcessCount(), -1);

  for (int k = 0; k < App().ProcessCount(); k++)
  {
    int loc = glob_linear_sweep_order[k];
    glob_order_mapping[loc] = k;
  }

  // Determine sweep order ranks
  App().Log().Log0Verbose1() << App().ProgramTimer().GetTimeString()
                             << " Determining sweep order ranks.";

  std::vector<int> glob_sweep_order_rank(App().ProcessCount(), -1);

  int abs_max_rank = 0;
  for (int k = 0; k < App().ProcessCount(); k++)
  {
    int loc = glob_linear_sweep_order[k];
    if (global_dependencies[loc].empty()) glob_sweep_order_rank[k] = 0;
    else
    {
      int max_rank = -1;
      for (auto dep_loc : global_dependencies[loc])
      {
        if (dep_loc < 0) continue;
        int dep_mapped_index = glob_order_mapping[dep_loc];

        if (glob_sweep_order_rank[dep_mapped_index] > max_rank)
          max_rank = glob_sweep_order_rank[dep_mapped_index];
      }
      glob_sweep_order_rank[k] = max_rank + 1;
      if ((max_rank + 1) > abs_max_rank) abs_max_rank = max_rank + 1;
    }
  }

  // Generate TDG structure
  App().Log().Log0Verbose1() << App().ProgramTimer().GetTimeString()
                             << " Generating TDG structure.";
  for (int r = 0; r <= abs_max_rank; r++)
  {
    chi_mesh::sweep_management::STDG new_stdg;

    for (int k = 0; k < App().ProcessCount(); k++)
    {
      if (glob_sweep_order_rank[k] == r) new_stdg.item_id.push_back(glob_linear_sweep_order[k]);
    }
    global_sweep_planes_.push_back(new_stdg);
  }
}

} // namespace chi_mesh::sweep_management
