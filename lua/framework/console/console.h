// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#pragma once

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "framework/parameters/parameter_block.h"
#include "framework/parameters/input_parameters.h"
#include "framework/logging/log_exceptions.h"
#include "framework/utils/utils.h"
#include <vector>
#include <string>
#include <map>
#include <stack>

namespace opensn
{
class Solver;
}

namespace opensnlua
{

struct RegistryStatuses;

/// Class for handling the console and scripting.
class Console
{
  /// Pointer to lua console state
  lua_State* console_state_;
  /// Buffer of commands to execute
  std::vector<std::string> command_buffer_;
  static Console instance_;

  Console() noexcept;

public:
  /// Access to the singleton
  static Console& GetInstance() noexcept;

  lua_State*& GetConsoleState() { return console_state_; }

  std::vector<std::string>& GetCommandBuffer() { return command_buffer_; }

  /// Executes the loop for the console.
  void RunConsoleLoop(char* fileName = nullptr) const;

  /**
   * Executes the given file in the Lua engine.
   * \author Jan
   */
  int ExecuteFile(const std::string& fileName, int argc, char** argv) const;

  /// Flushes any commands in the command buffer.
  void FlushConsole();

  /**
   * Makes a formatted output, readible by the documentation scripts, of all the lua wrapper
   * functions.
   */
  void DumpRegister() const;
};

extern Console& console;

} // namespace opensnlua
