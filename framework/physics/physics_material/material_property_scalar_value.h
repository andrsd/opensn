#pragma once

#include "framework/physics/physics_material/material_property_base.h"

namespace opensn
{
class App;
}

namespace chi_physics
{

/**Simple scalar material property.*/
class ScalarValue : public chi_physics::MaterialProperty
{
public:
  double value_ = 1.0;

  explicit ScalarValue(opensn::App& app) : MaterialProperty(app, PropertyType::SCALAR_VALUE) {}

  double GetScalarValue() override { return value_; }
  // FIXME
#if 0
  void PushLuaTable(lua_State* L) const override
  {
    lua_newtable(L);
    lua_pushstring(L, "is_empty");
    lua_pushboolean(L, false);
    lua_settable(L, -3);

    lua_pushstring(L, "value");
    lua_pushnumber(L, value_);
    lua_settable(L, -3);
  }
#endif
};

} // namespace chi_physics
