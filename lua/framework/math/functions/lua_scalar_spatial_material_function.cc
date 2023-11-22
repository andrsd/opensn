#include "lua/framework/math/functions/lua_scalar_spatial_material_function.h"
#include "framework/lua.h"
#include "framework/runtime.h"
#include "framework/console/console.h"

using namespace opensn;

namespace opensnlua
{

InputParameters
LuaScalarSpatialMaterialFunction::GetInputParameters()
{
  InputParameters params = ScalarSpatialMaterialFunction::GetInputParameters();
  params.AddRequiredParameter<std::string>("lua_function_name", "Name of the lua function");
  return params;
}

LuaScalarSpatialMaterialFunction::LuaScalarSpatialMaterialFunction(const InputParameters& params)
  : ScalarSpatialMaterialFunction(params),
    lua_function_name_(params.GetParamValue<std::string>("lua_function_name"))
{
}

double
LuaScalarSpatialMaterialFunction::Evaluate(int mat_id, const opensn::Vector3& xyz) const
{
  std::string fname = "LuaScalarSpatialMaterialFunction::Evaluate";
  lua_State* L = console.GetConsoleState();
  // Load lua function
  lua_getglobal(L, lua_function_name_.c_str());

  // Error check lua function
  if (not lua_isfunction(L, -1))
    throw std::logic_error(fname + " attempted to access lua-function, " + lua_function_name_ +
                           ", but it seems the function could not be retrieved.");

  // Push arguments
  lua_pushinteger(L, mat_id);
  lua_pushnumber(L, xyz.x);
  lua_pushnumber(L, xyz.y);
  lua_pushnumber(L, xyz.z);

  // Call lua function
  // 4 arguments, 1 result (double), 0=original error object
  double lua_return;
  if (lua_pcall(L, 4, 1, 0) == 0)
  {
    LuaCheckNumberValue(fname, L, -1);
    lua_return = lua_tonumber(L, -1);
  }
  else
    throw std::logic_error(fname + " attempted to call lua-function, " + lua_function_name_ +
                           ", but the call failed." + xyz.PrintStr());

  lua_pop(L, 1); // pop the double, or error code

  return lua_return;
}

} // namespace opensnlua
