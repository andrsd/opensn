// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#include "lua_material.h"
#include "framework/lua.h"
#include "framework/materials/material.h"
#include "framework/materials/multi_group_xs/multi_group_xs.h"
#include "framework/materials/material_property_scalar_value.h"
#include "framework/materials/material_property_isotropic_mg_src.h"
#include "framework/runtime.h"
#include "framework/logging/log.h"
#include "framework/console/console.h"
#include <iostream>

using namespace opensn;

namespace opensnlua
{

RegisterLuaFunctionNamespace(MatAddMaterial, mat, AddMaterial);
RegisterLuaFunctionNamespace(MatAddProperty, mat, AddProperty);
RegisterLuaFunctionNamespace(MatSetProperty, mat, SetProperty);
RegisterLuaFunctionNamespace(MatGetProperty, mat, GetProperty);

RegisterLuaConstantAsIs(SCALAR_VALUE, Varying(1));
RegisterLuaConstantAsIs(TRANSPORT_XSECTIONS, Varying(10));
RegisterLuaConstantAsIs(ISOTROPIC_MG_SOURCE, Varying(11));

namespace
{

void
ScalarPropertyPushTable(lua_State* L, std::shared_ptr<MaterialProperty> property)
{
  lua_newtable(L);
  LuaPushTableKey(L, "is_empty", false);
  LuaPushTableKey(L, "value", property->GetScalarValue());
}

void
IsotropicMGSourcePropertyPushTable(lua_State* L, std::shared_ptr<IsotropicMultiGrpSource> property)
{
  lua_newtable(L);
  LuaPushTableKey(L, "is_empty", false);
  LuaPushTableKey(L, "G", property->source_value_g.size());
  LuaPushTableKey(L, "source_value_g", property->source_value_g);
}

void
MaterialPropertyPushLuaTable(lua_State* L)
{
  lua_newtable(L);
  LuaPushTableKey(L, "is_empty", true);
}

void
PropertyPushLuaTable(lua_State* L, std::shared_ptr<MaterialProperty> property)
{
  if (property->Type() == PropertyType::SCALAR_VALUE)
    ScalarPropertyPushTable(L, property);
  else if (property->Type() == PropertyType::ISOTROPIC_MG_SOURCE)
    IsotropicMGSourcePropertyPushTable(
      L, std::dynamic_pointer_cast<IsotropicMultiGrpSource>(property));
  else
    MaterialPropertyPushLuaTable(L);
}

} // namespace

int
MatAddMaterial(lua_State* L)
{
  auto new_material = std::make_shared<Material>();
  new_material->name = LuaArgOptional<std::string>(L, 1, "");

  opensn::material_stack.push_back(new_material);

  const size_t index = opensn::material_stack.size() - 1;

  opensn::log.Log0Verbose1() << "New material added at index " << index << " with name \""
                             << new_material->name << "\"";

  return LuaReturn(L, index);
}

int
MatAddProperty(lua_State* L)
{
  const std::string fname = "mat.AddProperty";
  LuaCheckArgs<int, int>(L, fname);

  auto material_index = LuaArg<int>(L, 1);
  auto property_index = LuaArg<int>(L, 2);

  // Get reference to material
  auto cur_material = opensn::GetStackItemPtr(opensn::material_stack, material_index, fname);

  auto provided_name = LuaArgOptional<std::string>(
    L, 3, std::string("Property ") + std::to_string(cur_material->properties.size()));

  // Process property
  if (property_index == static_cast<int>(PropertyType::SCALAR_VALUE))
  {
    // Duplicates are allowed

    auto prop = std::make_shared<ScalarValue>();

    prop->property_name = provided_name;

    cur_material->properties.push_back(prop);
    opensn::log.Log0Verbose1() << "Scalar Value Property added to material at index "
                               << material_index;
  }
  else if (property_index == static_cast<int>(PropertyType::TRANSPORT_XSECTIONS))
  {
    // Check for duplicate
    for (int p = 0; p < cur_material->properties.size(); p++)
    {
      if (cur_material->properties[p]->Type() == PropertyType::TRANSPORT_XSECTIONS)
      {
        opensn::log.Log0Error() << "Material " << material_index << " \"" << cur_material->name
                                << "\""
                                << " already has property TRANSPORT_XSECTIONS" << std::endl;
        opensn::Exit(EXIT_FAILURE);
      }
    }

    auto prop = std::make_shared<MultiGroupXS>();

    prop->property_name = provided_name;

    cur_material->properties.push_back(prop);
    opensn::log.Log0Verbose1() << "Transport cross sections added to material at index "
                               << material_index;

    opensn::multigroup_xs_stack.push_back(prop);

    const size_t index = opensn::multigroup_xs_stack.size() - 1;
    return LuaReturn(L, index);
  }
  else if (property_index == static_cast<int>(PropertyType::ISOTROPIC_MG_SOURCE))
  {
    // Check for duplicate
    for (int p = 0; p < cur_material->properties.size(); p++)
    {
      if (cur_material->properties[p]->Type() == PropertyType::ISOTROPIC_MG_SOURCE)
      {
        opensn::log.Log0Error() << "Material " << material_index << " \"" << cur_material->name
                                << "\""
                                << " already has property ISOTROPIC_MG_SOURCE " << property_index
                                << std::endl;
        opensn::Exit(EXIT_FAILURE);
      }
    }

    auto prop = std::make_shared<IsotropicMultiGrpSource>();

    prop->property_name = provided_name;

    cur_material->properties.push_back(prop);
    opensn::log.Log0Verbose1() << "Isotropic Multigroup Source added to material at index "
                               << material_index;
  }
  else
  {
    opensn::log.Log0Error() << "Unsupported property type in call to mat.AddProperty.";
    opensn::Exit(EXIT_FAILURE);
  }

  return LuaReturn(L);
}

int
MatSetProperty(lua_State* L)
{
  const std::string fname = "mat.SetProperty";
  const int num_args = lua_gettop(L);

  if (num_args < 3)
  {
    opensn::log.Log0Error() << "Invalid number of arguments when calling mat.SetProperty";
    opensn::Exit(EXIT_FAILURE);
  }

  auto material_index = LuaArg<int>(L, 1);
  int property_index = -1;
  std::string property_index_name;
  if (lua_isnumber(L, 2))
    property_index = LuaArg<int>(L, 2);
  else
    property_index_name = LuaArg<std::string>(L, 2);

  auto operation_index = LuaArg<int>(L, 3);

  // Get reference to material
  auto cur_material = opensn::GetStackItemPtr(opensn::material_stack, material_index, fname);

  // If user supplied name then find property index
  if (not lua_isnumber(L, 2))
  {
    for (auto& property : cur_material->properties)
      if (property->property_name == property_index_name)
        property_index = static_cast<int>(property->Type());
  }

  // Process property
  if (property_index == static_cast<int>(PropertyType::SCALAR_VALUE))
  {
    int location_of_prop = -1;
    // Check if the material has this property
    if (lua_isnumber(L, 2))
    {
      for (int p = 0; p < cur_material->properties.size(); p++)
        if (cur_material->properties[p]->Type() == PropertyType::SCALAR_VALUE)
          location_of_prop = p;
    }
    else
    {
      for (int p = 0; p < cur_material->properties.size(); p++)
        if (cur_material->properties[p]->property_name == property_index_name)
          location_of_prop = p;
    }

    // If the property is valid
    if (location_of_prop >= 0)
    {
      auto prop = std::static_pointer_cast<ScalarValue>(cur_material->properties[location_of_prop]);

      // Process operation
      if (operation_index == static_cast<int>(OperationType::SINGLE_VALUE))
      {
        auto value = LuaArg<double>(L, 4);
        prop->value_ = value;
        opensn::log.Log0Verbose1()
          << "Scalar value for material at index " << material_index << " set to " << value;
      }
      else
      {
        opensn::log.Log0Error() << "ERROR: Unsupported operation for SCALAR_VALUE." << std::endl;
        opensn::Exit(EXIT_FAILURE);
      }
    }
    else
    {
      opensn::log.Log0Error() << "ERROR: Material has no property SCALAR_VALUE." << std::endl;
      opensn::Exit(EXIT_FAILURE);
    }
  } // if scalar value
  else if (property_index == static_cast<int>(PropertyType::TRANSPORT_XSECTIONS))
  {
    int location_of_prop = -1;
    // Check if the material has this property
    if (lua_isnumber(L, 2))
    {
      for (int p = 0; p < cur_material->properties.size(); p++)
      {
        if (cur_material->properties[p]->Type() == PropertyType::TRANSPORT_XSECTIONS)
        {
          location_of_prop = p;
        }
      }
    }
    else
    {
      for (int p = 0; p < cur_material->properties.size(); p++)
      {
        if (cur_material->properties[p]->property_name == property_index_name)
        {
          location_of_prop = p;
        }
      }
    }

    // If the property is valid
    if (location_of_prop >= 0)
    {
      auto prop =
        std::static_pointer_cast<MultiGroupXS>(cur_material->properties[location_of_prop]);

      // Process operation
      if (operation_index == static_cast<int>(OperationType::SIMPLE_ONE_GROUP))
      {
        if (num_args != 5)
          LuaPostArgAmountError("MatSetProperty", L, 5, num_args);

        auto sigma_t = LuaArg<double>(L, 4);
        auto c = LuaArg<double>(L, 5);

        prop->Initialize(sigma_t, c);
      }
      else if (operation_index == static_cast<int>(OperationType::OPENSN_XSFILE))
      {
        if (num_args != 4)
          LuaPostArgAmountError("MatSetProperty", L, 4, num_args);

        const auto file_name = LuaArg<std::string>(L, 4);

        prop->Initialize(file_name);
      }
      else if (operation_index == static_cast<int>(OperationType::OPENMC_XSLIB))
      {
        if (num_args < 5)
          LuaPostArgAmountError("MatSetProperty", L, 5, num_args);

        const auto file_name = LuaArg<std::string>(L, 4);
        const auto temperature = LuaArg<double>(L, 5);
        const auto dataset_name = LuaArgOptional<std::string>(L, 6, "set1");
        prop->Initialize(file_name, dataset_name, temperature);
      }
      else if (operation_index == static_cast<int>(OperationType::EXISTING))
      {
        if (num_args != 4)
          LuaPostArgAmountError("MatSetProperty", L, 4, num_args);

        auto handle = LuaArg<int>(L, 4);

        std::shared_ptr<MultiGroupXS> xs;
        try
        {
          xs = std::dynamic_pointer_cast<MultiGroupXS>(
            opensn::GetStackItemPtr(opensn::multigroup_xs_stack, handle, fname));
        }
        catch (const std::out_of_range& o)
        {
          opensn::log.LogAllError()
            << "ERROR: Invalid cross-section handle in call to MatSetProperty." << std::endl;
          opensn::Exit(EXIT_FAILURE);
        }
        //        auto old_prop = prop;
        prop = xs;

        cur_material->properties[location_of_prop] = prop;

        //        delete old_prop; //Still debating if this should be deleted
      }
      else
      {
        opensn::log.LogAllError() << "Unsupported operation for TRANSPORT_XSECTIONS." << std::endl;
        opensn::Exit(EXIT_FAILURE);
      }
    }
    else
    {
      opensn::log.LogAllError() << "Material has no property TRANSPORT_XSECTIONS." << std::endl;
      opensn::Exit(EXIT_FAILURE);
    }
  } // if thermal conductivity
  else if (property_index == static_cast<int>(PropertyType::ISOTROPIC_MG_SOURCE))
  {
    int location_of_prop = -1;
    // Check if the material has this property
    if (lua_isnumber(L, 2))
    {
      for (int p = 0; p < cur_material->properties.size(); p++)
      {
        if (cur_material->properties[p]->Type() == PropertyType::ISOTROPIC_MG_SOURCE)
        {
          location_of_prop = p;
        }
      }
    }
    else
    {
      for (int p = 0; p < cur_material->properties.size(); p++)
      {
        if (cur_material->properties[p]->property_name == property_index_name)
        {
          location_of_prop = p;
        }
      }
    }

    // If the property is valid
    if (location_of_prop >= 0)
    {
      auto prop = std::static_pointer_cast<IsotropicMultiGrpSource>(
        cur_material->properties[location_of_prop]);

      if (operation_index == static_cast<int>(OperationType::SINGLE_VALUE))
      {
        if (num_args != 4)
          LuaPostArgAmountError("MatSetProperty", L, 4, num_args);

        auto value = LuaArg<double>(L, 4);

        prop->source_value_g.resize(1, value);
        opensn::log.Log0Verbose1() << "Isotropic Multigroup Source value for material at index "
                                   << material_index << " set to " << value;
      }
      else if (operation_index == static_cast<int>(OperationType::FROM_ARRAY))
      {
        if (num_args != 4)
          LuaPostArgAmountError("MatSetProperty", L, 4, num_args);
        prop->source_value_g = LuaArg<std::vector<double>>(L, 4);
        opensn::log.Log0Verbose1() << "Isotropic Multigroup Source populated with "
                                   << prop->source_value_g.size() << " values";
      }
      else
      {
        opensn::log.LogAllError() << "Unsupported operation for ISOTROPIC_MG_SOURCE." << std::endl;
        opensn::Exit(EXIT_FAILURE);
      }
    }
    else
    {
      opensn::log.LogAllError() << "Material \"" << cur_material->name
                                << "\" has no property ISOTROPIC_MG_SOURCE." << std::endl;
      opensn::Exit(EXIT_FAILURE);
    }
  }
  else
  {
    opensn::log.LogAllError()
      << "Unsupported material property specified in call to MatSetProperty." << property_index
      << std::endl;
    opensn::Exit(EXIT_FAILURE);
  }

  return LuaReturn(L);
}

int
MatGetProperty(lua_State* L)
{
  const std::string fname = "mat.GetProperty";
  const int num_args = lua_gettop(L);
  if (num_args != 2)
    LuaPostArgAmountError("MatGetProperty", L, 2, num_args);

  auto material_index = LuaArg<int>(L, 1);
  int property_index = -1;
  std::string property_index_name;
  if (lua_isnumber(L, 2))
    property_index = LuaArg<int>(L, 2);
  else
    property_index_name = LuaArg<std::string>(L, 2);

  // Get reference to material
  auto cur_material = opensn::GetStackItemPtr(opensn::material_stack, material_index, fname);

  // If user supplied name then find property index
  if (not lua_isnumber(L, 2))
  {
    for (auto& property : cur_material->properties)
      if (property->property_name == property_index_name)
        property_index = static_cast<int>(property->Type());
  }

  // Process property
  bool property_populated = false;
  for (auto& property : cur_material->properties)
  {
    if (static_cast<int>(property->Type()) == property_index)
    {
      PropertyPushLuaTable(L, property);
      property_populated = true;
    }
  }

  if (not property_populated)
  {
    opensn::log.LogAllError() << "Invalid material property specified in call to MatGetProperty."
                              << property_index << std::endl;
    opensn::Exit(EXIT_FAILURE);
  }

  return 1;
}

} // namespace opensnlua
