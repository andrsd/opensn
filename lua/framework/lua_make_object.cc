#include "lua/framework/lua.h"

#include "framework/object_factory.h"
#include "lua/framework/console/console.h"

namespace opensnlua
{

/**Generic lua routine for the creation of objects.
 * \param params ParameterBlock A single block tree that requires a parameter
 *  called obj_type that indicates the type of object to make.
 */
int chiMakeObject(lua_State* L);

/**Generic lua routine for the creation of objects.
 * \param type string The type to create.
 * \param params ParameterBlock A single block tree.
 */
int chiMakeObjectType(lua_State* L);

RegisterLuaFunctionAsIs(chiMakeObject);
RegisterLuaFunctionAsIs(chiMakeObjectType);

int
chiMakeObject(lua_State* L)
{
  const std::string fname = __FUNCTION__;
  const int num_args = lua_gettop(L);
  if (num_args != 1) LuaPostArgAmountError(fname, 2, num_args);

  LuaCheckTableValue(fname, L, 1);

  const auto params = TableParserAsParameterBlock::ParseTable(L, 1);

  const auto& object_maker = opensn::ObjectFactory::GetInstance();
  auto object = object_maker.MakeRegisteredObject(params);
  opensn::object_stack.push_back(object);
  const size_t handle = opensn::object_stack.size() - 1;

  const std::string type = params.GetParamValue<std::string>("obj_type");

  lua_pushinteger(L, static_cast<lua_Integer>(handle));
  return 1;
}

int
chiMakeObjectType(lua_State* L)
{
  const std::string fname = __FUNCTION__;
  const int num_args = lua_gettop(L);
  if (num_args != 2) LuaPostArgAmountError(fname, 2, num_args);

  LuaCheckStringValue(fname, L, 1);
  LuaCheckTableValue(fname, L, 2);

  const std::string type = lua_tostring(L, 1);
  const auto params = TableParserAsParameterBlock::ParseTable(L, 2);

  const auto& object_maker = opensn::ObjectFactory::GetInstance();
  auto object = object_maker.MakeRegisteredObjectOfType(type, params);
  opensn::object_stack.push_back(object);
  auto handle = opensn::object_stack.size() - 1;

  lua_pushinteger(L, static_cast<lua_Integer>(handle));
  return 1;
}

} // namespace opensnlua
