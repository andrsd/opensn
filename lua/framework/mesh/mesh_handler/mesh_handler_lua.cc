#include "lua/base/lua.h"
#include "framework/app.h"
#include "framework/mesh/mesh_handler/mesh_handler.h"
#include "framework/runtime.h"
#include "framework/logging/log.h"
#include "mesh_handler_lua.h"
#include "lua/base/console.h"
#include <iostream>

RegisterLuaFunctionAsIs(chiMeshHandlerCreate);
RegisterLuaFunctionAsIs(chiMeshHandlerSetCurrent);
RegisterLuaFunctionAsIs(chiMeshHandlerExportMeshToObj);
RegisterLuaFunctionAsIs(chiMeshHandlerExportMeshToVTK);
RegisterLuaFunctionAsIs(chiMeshHandlerExportMeshToExodus);

int
chiMeshHandlerCreate(lua_State* L)
{
  // FIXME
  // int index = (int)chi_mesh::PushNewHandlerAndGetIndex();
  // lua_pushnumber(L, index);
  //
  // Chi::log.LogAllVerbose2() << "chiMeshHandlerCreate: Mesh Handler " << index << " created\n";

  return 1;
}

int
chiMeshHandlerSetCurrent(lua_State* L)
{
  // int num_args = lua_gettop(L);
  // if (num_args != 1) LuaPostArgAmountError("chiMeshHandlerSetCurrent", 1, num_args);
  //
  // int handle = lua_tonumber(L, 1);
  //
  // if ((handle < 0) or (handle >= Chi::meshhandler_stack.size()))
  // {
  //   // FIXME
  //   // Chi::log.LogAllError() << "Invalid handle to mesh handler specified "
  //   //                        << "in call to chiMeshHandlerSetCurrent";
  //   opensn::App::Exit(EXIT_FAILURE);
  // }
  //
  // Chi::current_mesh_handler = handle;
  //
  // // Chi::log.LogAllVerbose2() << "chiMeshHandlerSetCurrent: set to " << handle;
  //
  return 0;
}
