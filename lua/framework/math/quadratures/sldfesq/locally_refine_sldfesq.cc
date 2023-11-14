#include "lua/base/lua.h"
#include "lua/framework/runtime.h"
#include "framework/app.h"
#include "framework/math/quadratures/sldfesq/sldfe_sq.h"
#include "framework/logging/log.h"
#include "lua/base/console.h"
#include "sldfe_lua.h"

RegisterLuaFunctionAsIs(chiLocallyRefineSLDFESQAngularQuadrature);

int
chiLocallyRefineSLDFESQAngularQuadrature(lua_State* L)
{
  // int num_args = lua_gettop(L);
  // if ((num_args != 3) and (num_args != 4))
  //   LuaPostArgAmountError("chiLocallyRefineSLDFESQAngularQuadrature", 3, num_args);
  //
  // int handle = lua_tonumber(L, 1);
  //
  // chi_mesh::Vector3 ref_dir;
  // if (lua_istable(L, 2))
  // {
  //   lua_pushnumber(L, 1);
  //   lua_gettable(L, 2);
  //   ref_dir.x = lua_tonumber(L, -1);
  //   lua_pop(L, 1);
  //
  //   lua_pushnumber(L, 2);
  //   lua_gettable(L, 2);
  //   ref_dir.y = lua_tonumber(L, -1);
  //   lua_pop(L, 1);
  //
  //   lua_pushnumber(L, 3);
  //   lua_gettable(L, 2);
  //   ref_dir.z = lua_tonumber(L, -1);
  //   lua_pop(L, 1);
  // }
  // else
  // {
  //   // FIXME
  //   // Chi::log.LogAllError() << "chiLocallyRefineSLDFESQAngularQuadrature: "
  //   //                           "Second argument expected to be table {a,b,c}.";
  //   opensn::App::Exit(EXIT_FAILURE);
  // }
  //
  // double cone_size = lua_tonumber(L, 3);
  //
  // bool ref_dir_as_plane_normal = false;
  // if (num_args == 4) ref_dir_as_plane_normal = lua_toboolean(L, 4);
  //
  // try
  // {
  //   auto ref_quadrature = Chi::angular_quadrature_stack.at(handle);
  //   if (ref_quadrature->type_ == chi_math::AngularQuadratureType::SLDFESQ)
  //   {
  //     auto sldfesq =
  //       std::dynamic_pointer_cast<chi_math::SimplifiedLDFESQ::Quadrature>(ref_quadrature);
  //
  //     sldfesq->LocallyRefine(ref_dir, cone_size, ref_dir_as_plane_normal);
  //   }
  //   else
  //   {
  //     // FIXME
  //     // Chi::log.LogAllError() << "chiLocallyRefineSLDFESQAngularQuadrature: "
  //     //                           "Invalid angular quadrature type.";
  //     opensn::App::Exit(EXIT_FAILURE);
  //   }
  // }
  // catch (const std::out_of_range& o)
  // {
  //   // FIXME
  //   // Chi::log.LogAllError() << "chiLocallyRefineSLDFESQAngularQuadrature: "
  //   //                           "Invalid handle to angular quadrature.";
  //   opensn::App::Exit(EXIT_FAILURE);
  // }
  // catch (...)
  // {
  //   // FIXME
  //   // Chi::log.LogAllError() << "chiLocallyRefineSLDFESQAngularQuadrature: "
  //   //                           "Call failed with unknown error.";
  //   opensn::App::Exit(EXIT_FAILURE);
  // }
  //
  return 0;
}
