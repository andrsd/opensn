#include "framework/lua.h"
#include "framework/mesh/mesh_handler/mesh_handler.h"

#include "framework/physics/field_function/field_function_grid_based.h"

#include "framework/mesh/field_function_interpolation/ffinter_point.h"
#include "framework/mesh/field_function_interpolation/ffinter_slice.h"
#include "framework/mesh/field_function_interpolation/ffinter_line.h"
#include "framework/mesh/field_function_interpolation/ffinter_volume.h"
#include "lua/framework/math/functions/lua_scalar_material_function.h"
#include "framework/runtime.h"
#include "framework/logging/log.h"

#define dcastPoint(x) dynamic_cast<FieldFunctionInterpolationPoint&>(x)
#define dcastLine(x) dynamic_cast<FieldFunctionInterpolationLine&>(x)
#define dcastSlice(x) dynamic_cast<FieldFunctionInterpolationSlice&>(x)
#define dcastVolume(x) dynamic_cast<FieldFunctionInterpolationVolume&>(x)

#include "ffinterpol_lua.h"
#include "framework/console/console.h"

using namespace opensn;
using namespace opensnlua;

RegisterLuaFunctionAsIs(FFInterpolationSetProperty);
RegisterLuaConstantAsIs(PROBEPOINT, Varying(0));
RegisterLuaConstantAsIs(SLICE_POINT, Varying(1));
RegisterLuaConstantAsIs(SLICE_NORMAL, Varying(2));
RegisterLuaConstantAsIs(SLICE_TANGENT, Varying(3));
RegisterLuaConstantAsIs(SLICE_BINORM, Varying(4));
RegisterLuaConstantAsIs(OPERATION, Varying(5));
RegisterLuaConstantAsIs(OP_SUM, Varying(10));
RegisterLuaConstantAsIs(OP_AVG, Varying(11));
RegisterLuaConstantAsIs(OP_MAX, Varying(12));
RegisterLuaConstantAsIs(OP_SUM_FUNC, Varying(13));
RegisterLuaConstantAsIs(OP_AVG_FUNC, Varying(14));
RegisterLuaConstantAsIs(OP_MAX_FUNC, Varying(15));
RegisterLuaConstantAsIs(LOGICAL_VOLUME, Varying(8));

RegisterLuaConstantAsIs(ADD_FIELDFUNCTION, Varying(9));
RegisterLuaConstantAsIs(SET_FIELDFUNCTIONS, Varying(10));

RegisterLuaConstantAsIs(LINE_FIRSTPOINT, Varying(11));
RegisterLuaConstantAsIs(LINE_SECONDPOINT, Varying(12));
RegisterLuaConstantAsIs(LINE_NUMBEROFPOINTS, Varying(13));
RegisterLuaConstantAsIs(LINE_CUSTOM_ARRAY, Varying(14));

namespace
{

std::shared_ptr<LuaScalarMaterialFunction>
CreateFunction(const std::string& function_name)
{
  ParameterBlock blk;
  blk.AddParameter("lua_function_name", function_name);
  InputParameters params = LuaScalarMaterialFunction::GetInputParameters();
  params.AssignParameters(blk);
  return std::make_shared<LuaScalarMaterialFunction>(params);
}

} // namespace

int
FFInterpolationSetProperty(lua_State* L)
{
  const std::string fname = "FFInterpolationSetProperty";
  int numArgs = lua_gettop(L);

  // Get handle to field function
  const size_t ffihandle = lua_tonumber(L, 1);

  auto p_ffi = opensn::GetStackItemPtr(opensn::field_func_interpolation_stack, ffihandle, fname);

  // Process properties
  auto property = static_cast<Property>(lua_tonumber(L, 2));
  // Check point properties
  if (property == Property::PROBEPOINT)
    if (p_ffi->Type() != EType::POINT)
      throw std::logic_error(
        "Point property" + std::to_string(static_cast<int>(property)) +
        " used in FFInterpolationSetProperty but FFI is not a point-probe.");

  // Check slice properties
  if ((property >= Property::SLICEPOINT) && (property <= Property::SLICEBINORM))
    if (p_ffi->Type() != EType::SLICE)
      throw std::logic_error("Slice property" + std::to_string(static_cast<int>(property)) +
                             " used in FFInterpolationSetProperty but FFI is not a slice.");

  // Check Line properties
  if ((property >= Property::FIRSTPOINT) && (property <= Property::NUMBEROFPOINTS))
    if (p_ffi->Type() != EType::LINE)
      throw std::logic_error("Line property " + std::to_string(static_cast<int>(property)) +
                             " used in FFInterpolationSetProperty but FFI is not a line.");

  // Generic
  if (property == Property::ADD_FIELD_FUNCTION)
  {
    int ffhandle = lua_tonumber(L, 3);
    auto cur_ff_base = opensn::GetStackItemPtr(opensn::field_function_stack, ffhandle, fname);
    auto cur_ff = std::dynamic_pointer_cast<FieldFunctionGridBased>(cur_ff_base);

    p_ffi->GetFieldFunctions().push_back(cur_ff);
  }
  else if (property == Property::SET_FIELD_FUNCTIONS)
  {
    LuaCheckTableValue(fname, L, 3);
    std::vector<double> handle_array;
    LuaPopulateVectorFrom1DArray(fname, L, 3, handle_array);

    for (double handle_d : handle_array)
    {
      const auto ffhandle = static_cast<int>(handle_d);
      auto cur_ff_base = opensn::GetStackItemPtr(opensn::field_function_stack, ffhandle, fname);
      auto cur_ff = std::dynamic_pointer_cast<FieldFunctionGridBased>(cur_ff_base);

      p_ffi->GetFieldFunctions().push_back(cur_ff);
    } // for handle
  }
  else if (property == Property::PROBEPOINT)
  {
    auto& cur_ffi = dcastPoint(*p_ffi);

    double x = lua_tonumber(L, 3);
    double y = lua_tonumber(L, 4);
    double z = lua_tonumber(L, 5);

    cur_ffi.GetPointOfInterest() = Vector3(x, y, z);
  }
  else if (property == Property::SLICEPOINT)
  {
    auto& cur_ffi_slice = dcastSlice(*p_ffi);

    double x = lua_tonumber(L, 3);
    double y = lua_tonumber(L, 4);
    double z = lua_tonumber(L, 5);

    cur_ffi_slice.GetPlanePoint() = Vector3(x, y, z);
  }
  else if (property == Property::SLICENORMAL)
  {
    auto& cur_ffi_slice = dcastSlice(*p_ffi);

    double x = lua_tonumber(L, 3);
    double y = lua_tonumber(L, 4);
    double z = lua_tonumber(L, 5);

    cur_ffi_slice.GetNormal() = Vector3(x, y, z).Normalized();
  }
  else if (property == Property::SLICETANGENT)
  {
    auto& cur_ffi_slice = dcastSlice(*p_ffi);

    double x = lua_tonumber(L, 3);
    double y = lua_tonumber(L, 4);
    double z = lua_tonumber(L, 5);

    cur_ffi_slice.GetTangent() = Vector3(x, y, z).Normalized();
  }
  else if (property == Property::SLICEBINORM)
  {
    auto& cur_ffi_slice = dcastSlice(*p_ffi);

    double x = lua_tonumber(L, 3);
    double y = lua_tonumber(L, 4);
    double z = lua_tonumber(L, 5);

    cur_ffi_slice.GetBiNorm() = Vector3(x, y, z).Normalized();
  }
  else if (property == Property::FIRSTPOINT)
  {
    if (numArgs != 5) LuaPostArgAmountError("FFInterpolationSetProperty", 5, numArgs);

    auto& cur_ffi_line = dcastLine(*p_ffi);

    Vector3 point(lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5));
    cur_ffi_line.GetInitialPoint() = point;
  }
  else if (property == Property::SECONDPOINT)
  {
    if (numArgs != 5) LuaPostArgAmountError("FFInterpolationSetProperty", 5, numArgs);

    auto& cur_ffi_line = dcastLine(*p_ffi);

    Vector3 point(lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5));
    cur_ffi_line.GetFinalPoint() = point;
  }
  else if (property == Property::NUMBEROFPOINTS)
  {
    if (numArgs != 3) LuaPostArgAmountError("FFInterpolationSetProperty", 3, numArgs);

    auto& cur_ffi_line = dcastLine(*p_ffi);

    int num_points = lua_tonumber(L, 3);

    if (num_points < 2)
    {
      opensn::log.LogAllError() << "Line property FFI_LINE_NUMBEROFPOINTS"
                                << " used in FFInterpolationSetProperty. Number of points must"
                                << " be greater than or equal to 2.";
      opensn::Exit(EXIT_FAILURE);
    }
    cur_ffi_line.GetNumberOfPoints() = num_points;
  }
  else if (property == Property::CUSTOM_ARRAY)
  {
    if (numArgs != 3) LuaPostArgAmountError("FFInterpolationSetProperty", 3, numArgs);

    auto& cur_ffi_line = dcastLine(*p_ffi);

    if (not lua_istable(L, 3))
    {
      opensn::log.LogAllError() << "Line property FFI_LINE_CUSTOM_ARRAY"
                                << " used in FFInterpolationSetProperty. Argument 3 is expected "
                                   "to be an array.";
      opensn::Exit(EXIT_FAILURE);
    }

    const size_t table_len = lua_rawlen(L, 3);

    std::vector<double> new_array(table_len, 0.0);
    for (int k = 0; k < table_len; ++k)
    {
      lua_pushnumber(L, k + 1);
      lua_gettable(L, 3);
      new_array[k] = lua_tonumber(L, -1);
      lua_pop(L, 1);
    }

    cur_ffi_line.GetCustomArrays().push_back(new_array);
  }
  else if (property == Property::OPERATION)
  {
    if (numArgs != 3 and numArgs != 4)
      LuaPostArgAmountError("FFInterpolationSetProperty", 3, numArgs);

    if (p_ffi->Type() != EType::VOLUME)
      throw std::logic_error("Volume property FFI_PROP_OPERATION"
                             " used in FFInterpolationSetProperty can only be used with "
                             "Volume type interpolations.");

    auto& cur_ffi_volume = dcastVolume(*p_ffi);

    int op_type = lua_tonumber(L, 3);

    int OP_SUM = static_cast<int>(Operation::OP_SUM);
    int OP_MAX_FUNC = static_cast<int>(Operation::OP_MAX_FUNC);
    int OP_SUM_FUNC = static_cast<int>(Operation::OP_SUM_FUNC);

    if (!((op_type >= OP_SUM) && (op_type <= OP_MAX_FUNC)))
    {
      opensn::log.LogAllError() << "Volume property FFI_PROP_OPERATION"
                                << " used in FFInterpolationSetProperty. Unsupported OPERATON."
                                << " Supported types are OP_AVG and OP_SUM. " << op_type;
      opensn::Exit(EXIT_FAILURE);
    }

    if ((op_type >= OP_SUM_FUNC) and (op_type <= OP_MAX_FUNC))
    {
      if (numArgs != 4) LuaPostArgAmountError("FFInterpolationSetProperty", 4, numArgs);
      const char* func_name = lua_tostring(L, 4);
      auto operation_function = CreateFunction(func_name);
      opensn::function_stack.push_back(operation_function);
      cur_ffi_volume.SetOperationFunction(operation_function);
    }

    cur_ffi_volume.GetOperationType() = static_cast<Operation>(op_type);
  }
  else if (property == Property::LOGICAL_VOLUME)
  {
    if (numArgs != 3) LuaPostArgAmountError("FFInterpolationSetProperty", 3, numArgs);

    int logvol_hndle = lua_tonumber(L, 3);

    auto p_logical_volume = std::dynamic_pointer_cast<LogicalVolume>(
      opensn::GetStackItemPtr(opensn::object_stack, logvol_hndle, fname));

    if (p_ffi->Type() != EType::VOLUME)
      throw std::logic_error("Volume property FFI_PROP_LOGICAL_VOLUME"
                             " used in FFInterpolationSetProperty can only be used with "
                             "Volume type interpolations.");

    auto& cur_ffi_volume = dcastVolume(*p_ffi);

    cur_ffi_volume.GetLogicalVolume() = p_logical_volume;
  }
  else // Fall back
  {
    opensn::log.LogAllError() << "Invalid PropertyIndex used in FFInterpolationSetProperty.";
    opensn::Exit(EXIT_FAILURE);
  }

  return 0;
}
