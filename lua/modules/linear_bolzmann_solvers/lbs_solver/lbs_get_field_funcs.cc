// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#include "lbs_common_lua_functions.h"
#include "modules/linear_boltzmann_solvers/lbs_solver/lbs_solver.h"
#include "framework/field_functions/field_function_grid_based.h"
#include "framework/runtime.h"
#include "framework/logging/log.h"
#include "framework/lua.h"
#include "lua/framework/console/console.h"

using namespace opensn;

namespace opensnlua::lbs
{

RegisterLuaFunctionNamespace(LBSGetScalarFieldFunctionList, lbs, GetScalarFieldFunctionList);

int
LBSGetScalarFieldFunctionList(lua_State* L)
{
  const std::string fname = "lbs.GetScalarFieldFunctionList";
  LuaCheckArgs<size_t>(L, fname);

  // Get pointer to solver
  const auto solver_handle = LuaArg<size_t>(L, 1);
  const auto& lbs_solver =
    opensn::GetStackItem<opensn::lbs::LBSSolver>(opensn::object_stack, solver_handle, fname);

  /**Lambda for matching a field function smart pointer to one on
   * the runtime stack.*/
  auto GetStackFFHandle = [](std::shared_ptr<FieldFunctionGridBased>& local_ff)
  {
    size_t stack_ff_counter = 0;
    for (auto& stack_ff : opensn::field_function_stack)
    {
      if (stack_ff == local_ff)
        return stack_ff_counter;

      ++stack_ff_counter;
    }

    OpenSnLogicalError("Scalar field function lookup error");
  };

  // Building table of handles
  std::vector<size_t> ff_handles;
  // Flux moments first
  for (int g = 0; g < lbs_solver.NumGroups(); g++)
  {
    for (int m = 0; m < lbs_solver.NumMoments(); m++)
    {
      const size_t ff = lbs_solver.MapPhiFieldFunction(g, m);
      auto local_ff = lbs_solver.GetFieldFunctions()[ff];

      if (m != 0)
        continue;

      ff_handles.push_back(GetStackFFHandle(local_ff));
    }
  }

  //// Power generation
  // if (lbs_solver.Options().power_field_function_on)
  //{
  //   const size_t ff = lbs_solver.GetHandleToPowerGenFieldFunc();
  //   auto local_ff = lbs_solver.GetFieldFunctions()[ff];
  //
  //   lua_pushinteger(L, 1 + count++);
  //   lua_pushinteger(L, static_cast<lua_Integer>(GetStackFFHandle(local_ff)));
  //
  //   lua_settable(L, -3);
  // }

  return LuaReturn(L, ff_handles, ff_handles.size());
}

} // namespace opensnlua::lbs
