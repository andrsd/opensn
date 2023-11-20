#include "framework/lua.h"

#include "framework/mesh/unpartitioned_mesh/unpartitioned_mesh.h"
#include "framework/runtime.h"
#include "framework/logging/log.h"

#include "unpartition_mesh_lua_utils.h"
#include "framework/console/console.h"

using namespace opensn;

namespace opensnlua
{
RegisterLuaFunctionAsIs(chiCreateEmptyUnpartitionedMesh);
RegisterLuaFunctionAsIs(chiDestroyUnpartitionedMesh);

RegisterLuaFunctionAsIs(chiUnpartitionedMeshFromVTU);
RegisterLuaFunctionAsIs(chiUnpartitionedMeshFromPVTU);
RegisterLuaFunctionAsIs(chiUnpartitionedMeshFromEnsightGold);
RegisterLuaFunctionAsIs(chiUnpartitionedMeshFromWavefrontOBJ);
RegisterLuaFunctionAsIs(chiUnpartitionedMeshFromMshFormat);
RegisterLuaFunctionAsIs(chiUnpartitionedMeshFromExodusII);

int
chiCreateEmptyUnpartitionedMesh(lua_State* L)
{
  const std::string func_name = __FUNCTION__;

  opensn::Chi::unpartitionedmesh_stack.emplace_back(new UnpartitionedMesh());

  lua_pushnumber(L, static_cast<lua_Number>(opensn::Chi::unpartitionedmesh_stack.size() - 1));

  return 1;
}

int
chiDestroyUnpartitionedMesh(lua_State* L)
{
  const std::string func_name = __FUNCTION__;
  const int num_args = lua_gettop(L);
  if (num_args != 1) LuaPostArgAmountError(func_name, 1, num_args);

  LuaCheckNilValue(func_name, L, 1);

  const int handle = lua_tointeger(L, 1);

  auto mesh_ptr =
    opensn::Chi::GetStackItemPtr(opensn::Chi::unpartitionedmesh_stack, handle, func_name);

  mesh_ptr->CleanUp();
  opensn::Chi::unpartitionedmesh_stack[handle] = nullptr;

  opensn::Chi::log.Log() << "Unpartitioned mesh destroyed. Memory in use = "
                         << opensn::Chi::GetMemoryUsageInMB() << " MB";
  return 0;
}

int
chiUnpartitionedMeshFromVTU(lua_State* L)
{
  const std::string func_name = __FUNCTION__;
  int num_args = lua_gettop(L);
  if (num_args < 1) LuaPostArgAmountError(func_name, 1, num_args);

  LuaCheckNilValue(func_name, L, 1);
  if (num_args >= 2) LuaCheckNilValue(func_name, L, 2);

  const char* temp = lua_tostring(L, 1);
  const char* field = "";
  if (num_args >= 2) field = lua_tostring(L, 2);
  auto new_object = new UnpartitionedMesh;

  UnpartitionedMesh::Options options;
  options.file_name = std::string(temp);
  options.material_id_fieldname = field;
  options.boundary_id_fieldname = field;

  new_object->ReadFromVTU(options);

  opensn::Chi::unpartitionedmesh_stack.emplace_back(new_object);

  lua_pushnumber(L, static_cast<lua_Number>(opensn::Chi::unpartitionedmesh_stack.size() - 1));

  return 1;
}

int
chiUnpartitionedMeshFromPVTU(lua_State* L)
{
  const std::string func_name = __FUNCTION__;
  int num_args = lua_gettop(L);
  if (num_args < 1) LuaPostArgAmountError(func_name, 1, num_args);

  LuaCheckNilValue(func_name, L, 1);
  if (num_args >= 2) LuaCheckNilValue(func_name, L, 2);

  const char* temp = lua_tostring(L, 1);
  const char* field = "";
  if (num_args >= 2) field = lua_tostring(L, 2);
  auto new_object = new opensn::UnpartitionedMesh;

  opensn::UnpartitionedMesh::Options options;
  options.file_name = std::string(temp);
  options.material_id_fieldname = field;
  options.boundary_id_fieldname = field;

  new_object->ReadFromPVTU(options);

  opensn::Chi::unpartitionedmesh_stack.emplace_back(new_object);

  lua_pushnumber(L, static_cast<lua_Number>(opensn::Chi::unpartitionedmesh_stack.size() - 1));

  return 1;
}

int
chiUnpartitionedMeshFromEnsightGold(lua_State* L)
{
  const std::string func_name = __FUNCTION__;
  int num_args = lua_gettop(L);
  if (num_args < 1) LuaPostArgAmountError(func_name, 1, num_args);

  LuaCheckNilValue(func_name, L, 1);
  if (num_args >= 2) LuaCheckNilValue(func_name, L, 2);

  const char* temp = lua_tostring(L, 1);
  double scale = 1.0;
  if (num_args >= 2) scale = lua_tonumber(L, 2);
  auto new_object = new opensn::UnpartitionedMesh;

  opensn::UnpartitionedMesh::Options options;
  options.file_name = std::string(temp);
  options.scale = scale;

  new_object->ReadFromEnsightGold(options);

  opensn::Chi::unpartitionedmesh_stack.emplace_back(new_object);

  lua_pushnumber(L, static_cast<lua_Number>(opensn::Chi::unpartitionedmesh_stack.size() - 1));

  return 1;
}

int
chiUnpartitionedMeshFromWavefrontOBJ(lua_State* L)
{
  const std::string func_name = __FUNCTION__;
  int num_args = lua_gettop(L);
  if (num_args < 1) LuaPostArgAmountError(func_name, 1, num_args);

  LuaCheckNilValue(func_name, L, 1);

  const char* temp = lua_tostring(L, 1);

  auto new_object = new opensn::UnpartitionedMesh;

  opensn::UnpartitionedMesh::Options options;
  options.file_name = std::string(temp);

  new_object->ReadFromWavefrontOBJ(options);

  opensn::Chi::unpartitionedmesh_stack.emplace_back(new_object);

  lua_pushnumber(L, static_cast<lua_Number>(opensn::Chi::unpartitionedmesh_stack.size() - 1));

  return 1;
}

int
chiUnpartitionedMeshFromMshFormat(lua_State* L)
{
  const std::string func_name = __FUNCTION__;
  int num_args = lua_gettop(L);
  if (num_args < 1) LuaPostArgAmountError(func_name, 1, num_args);

  LuaCheckNilValue(func_name, L, 1);

  const char* temp = lua_tostring(L, 1);

  auto new_object = new opensn::UnpartitionedMesh;

  opensn::UnpartitionedMesh::Options options;
  options.file_name = std::string(temp);

  new_object->ReadFromMsh(options);

  opensn::Chi::unpartitionedmesh_stack.emplace_back(new_object);

  lua_pushnumber(L, static_cast<lua_Number>(opensn::Chi::unpartitionedmesh_stack.size() - 1));

  return 1;
}

int
chiUnpartitionedMeshFromExodusII(lua_State* L)
{
  const std::string func_name = __FUNCTION__;
  int num_args = lua_gettop(L);
  if (num_args < 1) LuaPostArgAmountError(func_name, 1, num_args);

  LuaCheckNilValue(func_name, L, 1);
  if (num_args >= 2) LuaCheckNilValue(func_name, L, 2);

  const char* temp = lua_tostring(L, 1);
  double scale = 1.0;
  if (num_args >= 2) scale = lua_tonumber(L, 2);
  auto new_object = new opensn::UnpartitionedMesh;

  opensn::UnpartitionedMesh::Options options;
  options.file_name = std::string(temp);
  options.scale = scale;

  new_object->ReadFromExodus(options);

  opensn::Chi::unpartitionedmesh_stack.emplace_back(new_object);

  lua_pushnumber(L, static_cast<lua_Number>(opensn::Chi::unpartitionedmesh_stack.size() - 1));

  return 1;
}

} // namespace opensnlua
