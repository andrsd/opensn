// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#include "lua/framework/lua.h"
#include "framework/console/console.h"
#include "framework/parameters/parameter_block.h"
#include "modules/linear_boltzmann_solvers/discrete_ordinates_solver/lbs_discrete_ordinates_solver.h"
#include "modules/linear_boltzmann_solvers/lbs_solver/groupset/lbs_groupset.h"
#include "lua/framework/mesh/lua_mesh.h"

using namespace opensn;

namespace opensnlua::lbs
{

int
LBSDiscreteOrdinatesSolverCreate(lua_State* L)
{
  const std::string fname = "lbs.DiscreteOrdinatesSolverCreate";
  LuaCheckArgs<ParameterBlock>(L, fname);

  auto params = LuaArg<ParameterBlock>(L, 1);

  auto grid_handle = params.GetParamValue<size_t>("grid_handle");
  auto solver_name = "LBSSolver";

  auto grid = opensnlua::MeshContinuumFromHandle(grid_handle);
  auto solver = std::make_shared<opensn::lbs::DiscreteOrdinatesSolver>(grid, solver_name);

  const size_t num_groups = params.GetParamValue<size_t>("num_groups");
  for (size_t g = 0; g < num_groups; ++g)
    solver->AddGroup(g);

  // Make groupsets
  const auto& groupsets_array = params.GetParam("groupsets");
  const size_t num_gs = groupsets_array.NumParameters();
  for (size_t gs = 0; gs < num_gs; ++gs)
  {
    const auto& groupset_params = groupsets_array.GetParam(gs);

    InputParameters gs_input_params = opensn::lbs::LBSGroupset::GetInputParameters();
    gs_input_params.SetObjectType("LBSSolver:LBSGroupset");
    gs_input_params.AssignParameters(groupset_params);

    opensn::lbs::LBSGroupset groupset(gs_input_params, gs, *solver);
    solver->AddGroupset(groupset);
  }

  // Options
  if (params.Has("options"))
  {
    auto options_params = opensn::lbs::LBSSolver::OptionsBlock();
    options_params.AssignParameters(params.GetParam("options"));

    solver->SetOptions(options_params);
  }

  if (params.Has("directions_sweep_order_to_print"))
    solver->SetVerboseSweepAngles(
      params.GetParamVectorValue<size_t>("directions_sweep_order_to_print"));
  if (params.Has("sweep_type"))
    solver->SetSweepType(params.GetParamValue<std::string>("sweep_type"));

  opensn::object_stack.push_back(solver);

  return LuaReturn(L, opensn::object_stack.size() - 1);
}

RegisterLuaFunctionNamespace(LBSDiscreteOrdinatesSolverCreate, lbs, DiscreteOrdinatesSolverCreate);

} // namespace opensnlua::lbs
