#pragma once

#include "luaaa/luaaa.hpp"
#include "framework/parameters/input_parameters.h"

namespace opensnluang
{

void ParseTableKeys(lua_State* L, int idx, opensn::ParameterBlock& block);

void Bind(lua_State* state);

} // namespace opensnluang

/// InputParameters
template <>
struct luaaa::LuaStack<opensn::InputParameters>
{
  inline static opensn::InputParameters get(lua_State* L, int idx)
  {
    opensn::InputParameters result;
    luaL_argcheck(L, lua_istable(L, idx), 1, "required table not found on stack.");
    if (lua_istable(L, idx)) opensnluang::ParseTableKeys(L, idx, result);
    // result.first = LuaStack<typename Container::first_type>::get(L, idx + 1);
    // result.second = LuaStack<typename Container::second_type>::get(L, idx + 2);
    return result;
  }

  inline static void put(lua_State* L, const opensn::InputParameters& params)
  {
    lua_newtable(L);
    std::cerr << "Not implemented" << std::endl;
    exit(-1);
    // std::cerr << "IP put on stack" << std::endl;
    // LuaStack<typename Container::first_type>::put(L, s.first);
    // lua_rawseti(L, -2, 1);
    // LuaStack<typename Container::second_type>::put(L, s.second);
    // lua_rawseti(L, -2, 2);
  }
};
