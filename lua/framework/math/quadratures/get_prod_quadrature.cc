#include "lua/base/lua.h"
#include "lua/framework/runtime.h"
#include "framework/app.h"
#include "framework/math/quadratures/angular_product_quadrature.h"
#include "framework/logging/log.h"
#include "quadratures_lua.h"
#include "lua/base/console.h"

RegisterLuaFunctionAsIs(chiGetProductQuadrature);

int
chiGetProductQuadrature(lua_State* L)
{
  // int num_args = lua_gettop(L);
  // if (num_args != 1) LuaPostArgAmountError("chiGetProductQuadrature", 1, num_args);
  //
  // int handle = lua_tonumber(L, 1);
  //
  // std::shared_ptr<chi_math::ProductQuadrature> quad;
  // try
  // {
  //   auto ang_quad = Chi::angular_quadrature_stack.at(handle);
  //   if (ang_quad->type_ == chi_math::AngularQuadratureType::ProductQuadrature)
  //     quad = std::static_pointer_cast<chi_math::ProductQuadrature>(ang_quad);
  //   else
  //   {
  //     // FIXME
  //     // Chi::log.LogAllError() << "chiGetProductQuadrature: Provided quadrature handle points to
  //     "
  //     //                           "a quadrature that is not a product quadrature.";
  //     opensn::App::Exit(EXIT_FAILURE);
  //   }
  // }
  // catch (const std::out_of_range& o)
  // {
  //   // FIXME
  //   // Chi::log.LogAllError() << "chiGetProductQuadrature: Invalid quadrature handle.";
  //   opensn::App::Exit(EXIT_FAILURE);
  // }
  //
  // lua_newtable(L);
  // for (size_t n = 0; n < quad->weights_.size(); ++n)
  // {
  //   lua_pushnumber(L, n + 1);
  //   lua_newtable(L);
  //
  //   lua_pushstring(L, "weight");
  //   lua_pushnumber(L, quad->weights_[n]);
  //   lua_settable(L, -3);
  //
  //   lua_pushstring(L, "polar");
  //   lua_pushnumber(L, quad->abscissae_[n].theta);
  //   lua_settable(L, -3);
  //
  //   lua_pushstring(L, "azimuthal");
  //   lua_pushnumber(L, quad->abscissae_[n].phi);
  //   lua_settable(L, -3);
  //
  //   lua_settable(L, -3);
  // }
  //
  return 1;
}
