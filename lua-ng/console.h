#pragma once

struct lua_State;

#include <vector>
#include <string>
#include <map>
#include <stack>

namespace opensnluang
{

/**
 * Class for handling the console and scripting.
 */
class Console
{
  /// Pointer to lua console state
  lua_State* console_state_;
  /// Buffer of commands to execute
  std::vector<std::string> command_buffer_;
  static Console instance_;

  Console() noexcept;

public:
  ~Console();

  /**
   * Access to the singleton
   */
  static Console& GetInstance() noexcept;

  lua_State*& GetConsoleState() { return console_state_; }

  std::vector<std::string>& GetCommandBuffer() { return command_buffer_; }

  /**
   * Executes the loop for the console.
   */
  void RunConsoleLoop(char* fileName = nullptr) const;

  /**
   * Executes the given file in the Lua engine.
   * \author Jan
   */
  int ExecuteFile(const std::string& fileName, int argc, char** argv) const;

  /**
   * Pushes location id and number of processes to lua state.
   */
  void PostMPIInfo(int location_id, int number_of_processes) const;

  /**
   * Flushes any commands in the command buffer.
   */
  void FlushConsole();
};

extern Console& console;

} // namespace opensnlua
