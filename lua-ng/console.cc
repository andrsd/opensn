#include "lua-ng/console.h"
#include "config.h"
#include "framework/object_factory.h"
#include "framework/runtime.h"
#include "framework/logging/log.h"
#include "framework/logging/log_exceptions.h"
#include "framework/mpi/mpi.h"
#include "framework/utils/utils.h"
#include "framework/runtime.h"
#include <iostream>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

using namespace opensn;

namespace opensnluang
{

Console& console = Console::GetInstance();

Console&
Console::GetInstance() noexcept
{
  static Console singleton;
  return singleton;
}

Console::Console() noexcept : console_state_(luaL_newstate())
{
  luaL_openlibs(console_state_);
}

Console::~Console()
{
//  luaL_
}

void
Console::FlushConsole()
{
  try
  {
    for (auto& command : command_buffer_)
    {
      bool error = luaL_dostring(console_state_, command.c_str());
      if (error)
      {
        opensn::log.LogAll() << lua_tostring(console_state_, -1);
        lua_pop(console_state_, 1);
      }
    }
  }
  catch (const std::exception& e)
  {
    opensn::log.LogAllError() << e.what();
    opensn::Exit(EXIT_FAILURE);
  }
}

void
Console::RunConsoleLoop(char*) const
{
  opensn::log.Log() << "Console loop started. "
                    << "Type \"exit\" to quit (or Ctl-C).";

  /** Wrapper to an MPI_Bcast call for a single integer
   * broadcast from location 0. */
  auto BroadcastSingleInteger = [](int* int_being_bcast)
  { MPI_Bcast(int_being_bcast, 1, MPI_INT, 0, mpi.comm); };

  /** Wrapper to an MPI_Bcast call for an array of characters
   * broadcast from location 0. */
  auto HomeBroadcastStringAsRaw = [](std::string string_to_bcast, int length)
  {
    char* raw_string_to_bcast = string_to_bcast.data();
    MPI_Bcast(raw_string_to_bcast, length, MPI_CHAR, 0, mpi.comm);
  };

  /** Wrapper to an MPI_Bcast call for an array of characters
   * broadcast from location 0. This call is for non-home locations. */
  auto NonHomeBroadcastStringAsRaw = [](std::string& string_to_bcast, int length)
  {
    std::vector<char> raw_chars(length + 1, '\0');
    MPI_Bcast(raw_chars.data(), length, MPI_CHAR, 0, mpi.comm);

    string_to_bcast = std::string(raw_chars.data());
  };

  /** Executes a string within the lua-console. */
  auto LuaDoString = [this](const std::string& the_string)
  {
    bool error = luaL_dostring(console_state_, the_string.c_str());
    if (error)
    {
      opensn::log.LogAll() << lua_tostring(console_state_, -1);
      lua_pop(console_state_, 1);
    }
  };

  auto ConsoleInputNumChars = [](const std::string& input)
  {
    int L = static_cast<int>(input.size());
    if (input == std::string("exit")) L = -1;

    return L;
  };

  const bool HOME = opensn::mpi.location_id == 0;

  while (true)
  {
    std::string console_input;

    if (HOME) std::cin >> console_input; // Home will be waiting here

    int console_input_len = ConsoleInputNumChars(console_input);

    BroadcastSingleInteger(&console_input_len); // Non-Home locs wait here

    if (console_input_len < 0) break;
    else if (HOME)
      HomeBroadcastStringAsRaw(console_input, console_input_len);
    else
      NonHomeBroadcastStringAsRaw(console_input, console_input_len);

    try
    {
      LuaDoString(console_input);
    }
    catch (const opensn::Chi::RecoverableException& e)
    {
      opensn::log.LogAllError() << e.what();
    }
    catch (const std::exception& e)
    {
      opensn::log.LogAllError() << e.what();
      Exit(EXIT_FAILURE);
    }
  } // while not termination posted

  opensn::log.Log() << "Console loop stopped successfully.";
}

int
Console::ExecuteFile(const std::string& fileName, int argc, char** argv) const
{
  lua_State* L = this->console_state_;
  if (not fileName.empty())
  {
    if (argc > 0)
    {
      lua_newtable(L);
      for (int i = 1; i <= argc; i++)
      {
        lua_pushnumber(L, i);
        lua_pushstring(L, argv[i - 1]);
        lua_settable(L, -3);
      }
      lua_setglobal(L, "chiArgs");
    }
    int error = luaL_dofile(this->console_state_, fileName.c_str());

    if (error > 0)
    {
      opensn::log.LogAllError() << "LuaError: " << lua_tostring(this->console_state_, -1);
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

void
Console::PostMPIInfo(int location_id, int number_of_processes) const
{
  lua_State* L = this->console_state_;

  lua_pushinteger(L, location_id);
  lua_setglobal(L, "chi_location_id");

  lua_pushinteger(L, number_of_processes);
  lua_setglobal(L, "chi_number_of_processes");
}

} // namespace opensnlua
