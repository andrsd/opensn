#include "lua_solver.h"
#include "framework/physics/solver_base/solver.h"
#include "framework/field_functions/field_function_grid_based.h"
#include "framework/event_system/physics_event_publisher.h"
#include "framework/object_factory.h"
#include "framework/runtime.h"
#include "framework/logging/log.h"
#include "framework/console/console.h"

using namespace opensn;

namespace opensnlua
{

RegisterLuaFunctionNamespace(SolverCreate, solver, Create);
RegisterLuaFunctionNamespace(SolverInitialize, solver, Initialize);
RegisterLuaFunctionNamespace(SolverExecute, solver, Execute);
RegisterLuaFunctionNamespace(SolverStep, solver, Step);
RegisterLuaFunctionNamespace(SolverAdvance, solver, Advance);
RegisterLuaFunctionNamespace(SolverSetBasicOption, solver, SetBasicOption);
RegisterLuaFunctionNamespace(SolverGetName, solver, GetName);
RegisterLuaFunctionNamespace(SolverGetFieldFunctionList, solver, GetFieldFunctionList);
RegisterLuaFunctionNamespace(SolverGetInfo, solver, GetInfo);
RegisterLuaFunctionNamespace(SolverSetProperties, solver, SetProperties);

int
SolverCreate(lua_State* L)
{
  const std::string fname = __FUNCTION__;
  const int num_args = lua_gettop(L);
  if (num_args != 1)
    LuaPostArgAmountError(fname, 1, num_args);

  LuaCheckTableValue(fname, L, 1);

  const auto params = TableParserAsParameterBlock::ParseTable(L, 1);

  const auto& object_maker = ObjectFactory::GetInstance();
  const size_t handle = object_maker.MakeRegisteredObject(params);

  lua_pushinteger(L, static_cast<lua_Integer>(handle));
  return 1;
}

int
SolverInitialize(lua_State* L)
{
  const std::string fname = __FUNCTION__;
  const int num_args = lua_gettop(L);

  if (num_args != 1)
    LuaPostArgAmountError(fname, 1, num_args);

  const auto solver_handle = LuaArg<size_t>(L, 1);

  auto& solver = opensn::GetStackItem<Solver>(opensn::object_stack, solver_handle, fname);

  PhysicsEventPublisher::GetInstance().SolverInitialize(solver);

  return 0;
}

int
SolverExecute(lua_State* L)
{
  const std::string fname = __FUNCTION__;
  const int num_args = lua_gettop(L);

  if (num_args != 1)
    LuaPostArgAmountError(fname, 1, num_args);

  const auto solver_handle = LuaArg<size_t>(L, 1);

  auto& solver = opensn::GetStackItem<Solver>(opensn::object_stack, solver_handle, fname);

  PhysicsEventPublisher::GetInstance().SolverExecute(solver);

  return 0;
}

int
SolverStep(lua_State* L)
{
  const std::string fname = __FUNCTION__;
  const int num_args = lua_gettop(L);

  if (num_args != 1)
    LuaPostArgAmountError(fname, 1, num_args);

  const auto solver_handle = LuaArg<size_t>(L, 1);

  auto& solver = opensn::GetStackItem<Solver>(opensn::object_stack, solver_handle, fname);

  PhysicsEventPublisher::GetInstance().SolverStep(solver);

  return 0;
}

int
SolverAdvance(lua_State* L)
{
  const std::string fname = __FUNCTION__;
  const int num_args = lua_gettop(L);

  if (num_args != 1)
    LuaPostArgAmountError(fname, 1, num_args);

  const auto solver_handle = LuaArg<size_t>(L, 1);

  auto& solver = opensn::GetStackItem<Solver>(opensn::object_stack, solver_handle, fname);

  PhysicsEventPublisher::GetInstance().SolverAdvance(solver);

  return 0;
}

int
SolverSetBasicOption(lua_State* L)
{
  const std::string fname = __FUNCTION__;
  const int num_args = lua_gettop(L);
  if (num_args != 3)
    LuaPostArgAmountError(fname, 3, num_args);

  const auto solver_handle = LuaArg<int>(L, 1);
  const auto option_name = LuaArg<std::string>(L, 2);
  auto& solver = opensn::GetStackItem<Solver>(opensn::object_stack, solver_handle, fname);

  try
  {
    auto& option = solver.GetBasicOptions()[option_name];

    switch (option.Type())
    {
      case VaryingDataType::VOID:
      case VaryingDataType::ARBITRARY_BYTES:
        throw std::logic_error("Solver:" + solver.TextName() + " option:" + option_name +
                               " is of invalid type."
                               " This indicates an implementation problem.");
      case VaryingDataType::STRING:
        option.SetStringValue(LuaArg<std::string>(L, 3));
        opensn::log.Log() << "Solver:" << solver.TextName() << " option:" << option_name
                          << " set to " << option.StringValue() << ".";
        break;
      case VaryingDataType::BOOL:
        option.SetBoolValue(LuaArg<bool>(L, 3));
        opensn::log.Log() << "Solver:" << solver.TextName() << " option:" << option_name
                          << " set to " << ((option.BoolValue()) ? "true" : "false") << ".";
        break;
      case VaryingDataType::INTEGER:
        option.SetIntegerValue(LuaArg<int>(L, 3));
        opensn::log.Log() << "Solver:" << solver.TextName() << " option:" << option_name
                          << " set to " << option.IntegerValue() << ".";
        break;
      case VaryingDataType::FLOAT:
        option.SetFloatValue(LuaArg<double>(L, 3));
        opensn::log.Log() << "Solver:" << solver.TextName() << " option:" << option_name
                          << " set to " << option.FloatValue() << ".";
        break;
    }
  }
  catch (const std::out_of_range& oor)
  {
    opensn::log.Log0Error() << fname << ": " << oor.what();
    throw oor;
  }

  return 0;
}

int
SolverGetName(lua_State* L)
{
  const std::string fname = "SolverGetName";
  const int num_args = lua_gettop(L);

  if (num_args != 1)
    LuaPostArgAmountError(fname, 1, num_args);

  const auto solver_handle = LuaArg<int>(L, 1);
  const auto& solver = opensn::GetStackItem<Solver>(opensn::object_stack, solver_handle, fname);

  lua_pushstring(L, solver.TextName().c_str());

  return 1;
}

int
SolverGetFieldFunctionList(lua_State* L)
{
  const std::string fname = __FUNCTION__;
  const int num_args = lua_gettop(L);
  if (num_args != 1)
    LuaPostArgAmountError("GetFieldFunctionList", 1, num_args);

  // Getting solver
  const auto solver_handle = LuaArg<size_t>(L, 1);
  const auto& solver = opensn::GetStackItem<Solver>(opensn::object_stack, solver_handle, fname);

  // Push up new table
  lua_newtable(L);
  for (size_t ff = 0; ff < solver.GetFieldFunctions().size(); ff++)
  {
    lua_pushinteger(L, static_cast<lua_Integer>(ff) + 1);
    int pff_count = -1;
    bool found = false;
    for (auto& pff : opensn::field_function_stack) // pff pointer to field func
    {
      ++pff_count;
      if (pff == solver.GetFieldFunctions()[ff])
      {
        lua_pushinteger(L, pff_count);
        found = true;
        break;
      }
    }

    if (not found)
      throw std::logic_error(fname + ": The solver specified has no "
                                     "field functions that match the global"
                                     " stack.");
    lua_settable(L, -3);
  }

  lua_pushinteger(L, static_cast<lua_Integer>(solver.GetFieldFunctions().size()));

  return 2;
}

int
SolverGetInfo(lua_State* L)
{
  const std::string fname = "SolverGetInfo";
  const int num_args = lua_gettop(L);

  if (num_args != 2)
    LuaPostArgAmountError(fname, 2, num_args);

  const auto solver_handle = LuaArg<size_t>(L, 1);
  const auto& solver = opensn::GetStackItem<Solver>(opensn::object_stack, solver_handle, fname);

  ParameterBlock params;
  if (lua_isstring(L, 2))
    params.AddParameter("name", LuaArg<std::string>(L, 2));
  else if (lua_istable(L, 2))
    params = TableParserAsParameterBlock::ParseTable(L, 2);
  else
    OpenSnInvalidArgument("Argument 2 can only take a string or a table");

  const auto output_params = solver.GetInfo(params);

  PushParameterBlock(L, output_params);

  const int num_sub_params = static_cast<int>(output_params.NumParameters());

  return output_params.IsScalar() ? 1 : num_sub_params;
}

int
SolverSetProperties(lua_State* L)
{
  const std::string fname = "SolverSetProperties";
  const int num_args = lua_gettop(L);
  if (num_args != 2)
    LuaPostArgAmountError(fname, 2, num_args);

  const auto solver_handle = LuaArg<size_t>(L, 1);
  auto& solver = opensn::GetStackItem<Solver>(opensn::object_stack, solver_handle, fname);

  LuaCheckTableValue(fname, L, 2);
  auto property_block = TableParserAsParameterBlock::ParseTable(L, 2);

  solver.SetProperties(property_block);

  return 0;
}

} // namespace opensnlua