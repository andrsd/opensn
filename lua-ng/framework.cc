#include "lua-ng/framework.h"
// #include "framework/mesh/mesh_generator/orthogonal_mesh_generator.h"
#include <iostream>

using namespace luaaa;
// using namespace opensn;

namespace opensnluang
{

class Cat
{
public:
  Cat() {}

  void meow() { std::cerr << "meow" << std::endl; }
};

void
BindFramework(lua_State* state)
{
  LuaClass<Cat> luaCat(state, "Cat");
  luaCat.ctor();
  luaCat.fun("meow", &Cat::meow);

  //  LuaClass<OrthogonalMeshGenerator> luaOrthogonalMeshGenerator(state,
  //  "OrthogonalMeshGenerator"); luaOrthogonalMeshGenerator

  // clang-format off
//  LuaClass<OrthogonalMeshGenerator> luaOrthogonalMeshGenerator(state,
///"OrthogonalMeshGenerator"); /  luaOrthogonalMeshGenerator /    .ctor<const InputParameters&>() /
///.fun("Execute", &OrthogonalMeshGenerator::Execute); /  // clang-format on /  module->def("A",
///&luaOrthogonalMeshGenerator); /  //  module->
}

} // namespace opensnluang
