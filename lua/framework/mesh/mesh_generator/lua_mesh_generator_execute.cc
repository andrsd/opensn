// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#include "framework/lua.h"
#include "framework/mesh/mesh_generator/mesh_generator.h"
#include "framework/console/console.h"
#include "framework/runtime.h"
#include "lua/framework/mesh/lua_mesh.h"

using namespace opensn;

namespace opensnlua
{

int MeshGeneratorExecute(lua_State* L);

RegisterLuaFunctionNamespace(MeshGeneratorExecute, mesh::MeshGenerator, Execute);

int
MeshGeneratorExecute(lua_State* L)
{
  const std::string fname = "mesh.MeshGenerator.Execute";
  LuaCheckArgs<size_t>(L, fname);

  auto handle = LuaArg<size_t>(L, 1);
  auto& generator = opensn::GetStackItem<MeshGenerator>(opensn::object_stack, handle, fname);
  auto grid = generator.Execute();
  auto grid_handle = opensnlua::AddMeshContinuum(grid);
  return LuaReturn(L, grid_handle);
}

} // namespace opensnlua
