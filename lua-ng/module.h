#pragma once

#include "luaaa/luaaa.hpp"
#include "framework/parameters/input_parameters.h"

namespace opensnluang
{

void ParseTableKeys(lua_State* L, int idx, opensn::ParameterBlock& block);
void Bind(lua_State* state);

} // namespace opensnluang
