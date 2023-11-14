#pragma once

#include "framework/app.h"
#include "lua/base/console.h"

class ConsoleApp : public opensn::App
{
public:
  explicit ConsoleApp(MPI_Comm comm);

  int Run(int argc, char** argv);

protected:
  /**
   * Parses input arguments
   *
   * \param argc int Number of arguments supplied.
   * \param argv char** Array of strings representing each argument.
   */
  void ParseArguments(int argc, char** argv);

  /**
   * Print program header
   * @param mode Mode we are running in
   * @param n_args Number of command line arguments
   */
  void PrintHeader(const std::string& mode, int n_args);

  /**
   * Run the in interactive mode
   */
  int RunInteractive(int argc, char** argv);

  /**
   * Run in batch mode
   */
  int RunBatch(int argc, char** argv);

  /**
   * Get version as a string
   * @return openSn version
   */
  std::string GetVersionStr();

private:
  bool termination_posted_;
  std::string input_file_name_;
  bool sim_option_interactive_;
  bool allow_petsc_error_handler_;
  bool supress_beg_end_timelog_;
  bool suppress_color_;
  bool dump_registry_;

  Console console_;
};
