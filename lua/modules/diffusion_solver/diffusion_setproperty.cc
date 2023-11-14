#include "lua/base/lua.h"
#include "modules/diffusion_solver/diffusion_solver.h"
#include "framework/runtime.h"
#include "framework/logging/log.h"

int
chiDiffusionSetProperty(lua_State* L)
{
  // FIXME
  // const std::string fname = __FUNCTION__;
  // const int num_args = lua_gettop(L);
  // if (num_args < 2) LuaPostArgAmountError(fname, num_args, 2);
  //
  // LuaCheckNilValue(fname, L, 1);
  // LuaCheckNilValue(fname, L, 2);
  //
  // // Get solver
  // LuaCheckNumberValue(fname, L, 1);
  // const int solver_index = lua_tonumber(L, 1);
  //
  // auto& solver = Chi::GetStackItem<chi_diffusion::Solver>(Chi::object_stack, solver_index,
  // fname);
  //
  // // Get property index
  // LuaCheckStringValue(fname, L, 2);
  // const std::string property_name = lua_tostring(L, 2);
  //
  // // Handle properties
  // if (property_name == "boundary_type")
  // {
  //   if (num_args < 4)
  //   {
  //     Chi::log.Log0Error() << "Invalid amount of arguments used in"
  //                          << " chiDiffusionSetProperty(...,\"boundary_type\".... "
  //                          << " At least 4 arguments are expected.";
  //     Chi::Exit(EXIT_FAILURE);
  //   }
  //   LuaCheckStringValue(fname, L, 3);
  //   const std::string bound_name = lua_tostring(L, 3);
  //
  //   LuaCheckStringValue(fname, L, 4);
  //   const std::string type_name = lua_tostring(L, 4);
  //
  //   if (type_name == "reflecting")
  //   {
  //     if (num_args != 4)
  //     {
  //       Chi::log.Log0Error() << "Invalid amount of arguments used in"
  //                            << " chiDiffusionSetProperty(...,\"boundary_type\",\"" << bound_name
  //                            << "\",\"reflecting\". "
  //                            << " 4 arguments are expected.";
  //       Chi::Exit(EXIT_FAILURE);
  //     }
  //
  //     chi_diffusion::Solver::BoundaryInfo bndry_info;
  //     bndry_info.first = chi_diffusion::BoundaryType::Reflecting;
  //
  //     solver.boundary_preferences_.insert(std::make_pair(bound_name, bndry_info));
  //
  //     Chi::log.Log() << "Boundary \"" << bound_name << "\" set as "
  //                    << "Reflecting.";
  //   }
  //   else if (type_name == "dirichlet")
  //   {
  //     if (num_args != 5)
  //     {
  //       Chi::log.Log0Error() << "Invalid amount of arguments used in"
  //                            << " chiDiffusionSetProperty(...,\"boundary_type\",\"" << bound_name
  //                            << "\",\"dirichlet\". "
  //                            << " 5 arguments are expected.";
  //       Chi::Exit(EXIT_FAILURE);
  //     }
  //     LuaCheckNumberValue(fname, L, 5);
  //     double b_value = lua_tonumber(L, 5);
  //
  //     chi_diffusion::Solver::BoundaryInfo bndry_info;
  //     bndry_info.first = chi_diffusion::BoundaryType::Dirichlet;
  //     bndry_info.second = {b_value};
  //     solver.boundary_preferences_.insert(std::make_pair(bound_name, bndry_info));
  //
  //     Chi::log.Log() << "Boundary \"" << bound_name << "\" set as "
  //                    << "Dirichlet with value " << b_value;
  //   }
  //   else if (type_name == "neumann")
  //   {
  //     if (num_args != 5)
  //     {
  //       Chi::log.Log0Error() << "Invalid amount of arguments used in"
  //                            << " chiDiffusionSetProperty(...,\"boundary_type\",\"" << bound_name
  //                            << "\",\"neumann\". "
  //                            << " 5 arguments are expected.";
  //       Chi::Exit(EXIT_FAILURE);
  //     }
  //     LuaCheckNumberValue(fname, L, 5);
  //     double f_value = lua_tonumber(L, 5);
  //
  //     chi_diffusion::Solver::BoundaryInfo bndry_info;
  //     bndry_info.first = chi_diffusion::BoundaryType::Robin;
  //     bndry_info.second = {0.0, 1.0, f_value};
  //     solver.boundary_preferences_.insert(std::make_pair(bound_name, bndry_info));
  //
  //     Chi::log.Log() << "Boundary \"" << bound_name << "\" set as "
  //                    << "Neumann with f = (" << f_value << ") ";
  //   }
  //   else if (type_name == "vacuum")
  //   {
  //     if (num_args != 4)
  //     {
  //       Chi::log.Log0Error() << "Invalid amount of arguments used in"
  //                            << " chiDiffusionSetProperty(...,\"boundary_type\",\"" << bound_name
  //                            << "\",\"vacuum\". "
  //                            << " 4 arguments are expected.";
  //       Chi::Exit(EXIT_FAILURE);
  //     }
  //
  //     chi_diffusion::Solver::BoundaryInfo bndry_info;
  //     bndry_info.first = chi_diffusion::BoundaryType::Robin;
  //     bndry_info.second = {0.25, 0.5, 0.0};
  //     solver.boundary_preferences_.insert(std::make_pair(bound_name, bndry_info));
  //
  //     Chi::log.Log() << "Boundary \"" << bound_name << "\" set as "
  //                    << "Vacuum.";
  //   }
  //   else if (type_name == "robin")
  //   {
  //     if (num_args != 7)
  //     {
  //       Chi::log.Log0Error() << "Invalid amount of arguments used in"
  //                            << " chiDiffusionSetProperty(...,\"boundary_type\",\"" << bound_name
  //                            << "\",\"robin\". "
  //                            << " 7 arguments are expected.";
  //       Chi::Exit(EXIT_FAILURE);
  //     }
  //     LuaCheckNumberValue(fname, L, 5);
  //     LuaCheckNumberValue(fname, L, 6);
  //     LuaCheckNumberValue(fname, L, 7);
  //
  //     double a_value = lua_tonumber(L, 5);
  //     double b_value = lua_tonumber(L, 6);
  //     double f_value = lua_tonumber(L, 7);
  //
  //     chi_diffusion::Solver::BoundaryInfo bndry_info;
  //     bndry_info.first = chi_diffusion::BoundaryType::Robin;
  //     bndry_info.second = {a_value, b_value, f_value};
  //     solver.boundary_preferences_.insert(std::make_pair(bound_name, bndry_info));
  //
  //     Chi::log.Log() << "Boundary \"" << bound_name << "\" set as "
  //                    << "Robin with a,b,f = (" << a_value << "," << b_value << "," << f_value
  //                    << ") ";
  //   }
  //   else
  //   {
  //     Chi::log.LogAllError() << "Unsupported boundary type encountered in call to "
  //                            << "chiDiffusionSetProperty(..,\"boundary_type\",.. :" << type_name;
  //     Chi::Exit(EXIT_FAILURE);
  //   }
  // }
  // else
  // {
  //   Chi::log.Log0Error() << "Invalid property in chiDiffusionSetProperty.";
  //   Chi::Exit(EXIT_FAILURE);
  // }
  return 0;
}
