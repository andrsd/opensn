#include "framework/lua.h"

#include "framework/mesh/surface_mesher/surface_mesher.h"
#include "framework/mesh/mesh_handler/mesh_handler.h"

#include "framework/runtime.h"
#include "framework/logging/log.h"
#include "surf_mesher_lua.h"
#include "framework/console/console.h"

using namespace opensn;

RegisterLuaFunctionAsIs(chiSurfaceMesherExecute);

int
chiSurfaceMesherExecute(lua_State* L)
{
  auto& cur_hndlr = GetCurrentHandler();
  opensn::Chi::log.LogAllVerbose2() << "Executing surface mesher\n";

  cur_hndlr.GetSurfaceMesher().Execute();

  opensn::Chi::log.LogAllVerbose2()
    << "chiSurfaceMesherExecute: Surface mesher execution completed." << std::endl;

  return 0;
}
