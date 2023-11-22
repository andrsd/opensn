#include "lua-ng/module.h"
#include "lua-ng/framework.h"
#include "config.h"

#include "framework/object.h"
#include "framework/object_factory.h"

using namespace luaaa;

namespace opensnluang
{

class Dog : public opensn::Object
{
public:
  Dog() : breed_("simple") {}

  Dog(const opensn::InputParameters& params)
    : opensn::Object(params), breed_(params.GetParamValue<std::string>("breed"))
  {
  }

  void Bark() { std::cout << breed_ << ": bark" << std::endl; }

private:
  std::string breed_;

public:
  static opensn::InputParameters GetInputParameters();
};

opensn::InputParameters
Dog::GetInputParameters()
{
  auto params = opensn::Object::GetInputParameters();
  params.AddOptionalParameter<std::string>("breed", "asdf", "");
  return params;
}

OpenSnRegisterObject(opensn, Dog);

// opensn lua module
std::shared_ptr<LuaModule> module;

void
Bind(lua_State* state)
{
  module = std::make_shared<LuaModule>(state, "opensn");
  module->def("version", PROJECT_VERSION);

  //  BindFramework(state);

  LuaClass<Dog> l_dog(state, "Dog");
  l_dog.ctor();
  l_dog.ctor<const opensn::InputParameters&>("Create");
  l_dog.fun("Bark", &Dog::Bark);
}

//

void
ParseTableValues(lua_State* L, opensn::ParameterBlock& block, const std::string& key)
{
  switch (lua_type(L, -1))
  {
    case LUA_TNIL:
      throw std::logic_error(std::string(__PRETTY_FUNCTION__) +
                             ": Encountered nil value assigned to key " + key);

    case LUA_TBOOLEAN:
      block.AddParameter(key, LuaStack<bool>::get(L, -1));
      break;

    case LUA_TNUMBER:
      if (lua_isinteger(L, -1)) block.AddParameter(key, LuaStack<int64_t>::get(L, -1));
      else
        block.AddParameter(key, LuaStack<double>::get(L, -1));
      break;

    case LUA_TSTRING:
      block.AddParameter(key, LuaStack<std::string>::get(L, -1));
      break;

    case LUA_TTABLE:
    {
      opensn::ParameterBlock new_block(key);
      ParseTableKeys(L, lua_gettop(L), new_block);
      block.AddParameter(new_block);
      break;
    }

    default:
      throw std::logic_error(std::string(__PRETTY_FUNCTION__) +
                             ": Encountered unsupported value type " +
                             lua_typename(L, lua_type(L, -2)) + " for key " + key);
  }
}

void
ParseTableKeys(lua_State* L, int idx, opensn::ParameterBlock& block)
{
  bool number_key_encountered = false;
  bool string_key_encountered = false;
  int key_number_index = 0;

  lua_pushnil(L);
  while (lua_next(L, idx) != 0)
  {
    const int top = lua_gettop(L);
    if (lua_type(L, -2) == LUA_TSTRING)
    {
      if (number_key_encountered)
        throw std::logic_error(std::string(__PRETTY_FUNCTION__) +
                               ": Encountered mixed key types (string and number)");

      string_key_encountered = true;
      const std::string key = LuaStack<std::string>::get(L, top - 1);
      ParseTableValues(L, block, key);
    } // if key is string

    if (lua_type(L, -2) == LUA_TNUMBER)
    {
      // If the key is a number then the following apply:
      // - This must be an array of items
      // - All the keys in the table must be numbers
      if (string_key_encountered)
        throw std::logic_error(std::string(__PRETTY_FUNCTION__) +
                               ": Encountered mixed key types (string and number)");

      if (block.Type() != opensn::ParameterBlockType::ARRAY) block.ChangeToArray();

      number_key_encountered = true;
      const std::string key = std::to_string(key_number_index);
      ParseTableValues(L, block, key);
      ++key_number_index;
    }

    lua_pop(L, 1);
  }
}

} // namespace opensnluang
