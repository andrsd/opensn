#pragma once

#include "framework/chi_lua.h"
#include "framework/parameters/input_parameters.h"

namespace lbs
{
class LBSSolver;
}

namespace lbs::common_lua_utils
{

// void SetBoundaryOptions(LBSSolver& lbs_solver,
//                         const chi_objects::InputParameters& params);

chi::InputParameters GetSyntax_SetOptions();
chi::ParameterBlock SetOptions(const chi::InputParameters& params);

int chiLBSSetOptions(lua_State* L);
int chiLBSSetPhiFromFieldFunction(lua_State* L);
void RegisterLuaEntities(lua_State* L);
} // namespace lbs::common_lua_utils
