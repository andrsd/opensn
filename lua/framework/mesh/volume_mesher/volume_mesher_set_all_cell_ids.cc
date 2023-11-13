#include "lua/base/lua.h"
#include "framework/mesh/volume_mesher/volume_mesher.h"
#include "volume_mesher_lua.h"
#include "lua/base/console.h"

RegisterLuaFunctionAsIs(chiVolumeMesherSetMatIDToAll);

int
chiVolumeMesherSetMatIDToAll(lua_State* L)
{
  // FIXME
  // int num_args = lua_gettop(L);
  // if (num_args != 1) LuaPostArgAmountError(__FUNCTION__, 1, num_args);
  //
  // LuaCheckNilValue(__FUNCTION__, L, 1);
  //
  // int mat_id = lua_tonumber(L, 1);
  //
  // chi_mesh::VolumeMesher::SetMatIDToAll(mat_id);
  return 0;
}
