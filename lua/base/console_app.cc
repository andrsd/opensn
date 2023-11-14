#include "lua/base/console_app.h"
#include "config.h"
#include "framework/event_system/event.h"
#ifndef NDEBUG
#include <unistd.h>
#endif

namespace
{
const std::string command_line_help_string =
  "\nUsage: exe inputfile [options values]\n"
  "\n"
  "     -v                          Level of verbosity. Default 0.\n"
  "                                 Can be either 0, 1 or 2.\n"
  "     a=b                         Executes argument as a lua string. "
  "i.e. x=2 or y=[[\"string\"]]\n"
  "     --allow_petsc_error_handler Allow petsc error handler.\n"
  "     --supress_beg_end_timelog   Suppress time logs at the \n"
  "                                 beginning and end of execution.\n"
  "     --suppress_color            Suppresses the printing of color.\n"
  "                                 useful for unit tests requiring a diff.\n"
  "     --dump-object-registry      Dumps the object registry.\n"
  "\n\n\n";
}

ConsoleApp::ConsoleApp(MPI_Comm comm)
  : opensn::App(comm),
    termination_posted_(false),
    sim_option_interactive_(true),
    allow_petsc_error_handler_(false),
    supress_beg_end_timelog_(false),
    suppress_color_(false),
    dump_registry_(false),
    console_(*this)
{
}

int
ConsoleApp::Run(int argc, char** argv)
{
  ParseArguments(argc, argv);

  auto& t_main = Log().CreateTimingBlock("ChiTech");
  t_main.TimeSectionBegin();
  SystemWideEventPublisher().PublishEvent(opensn::Event("ProgramStart"));

  int error_code = 0;
  if (!termination_posted_)
  {
    if (sim_option_interactive_) error_code = RunInteractive(argc, argv);
    else
      error_code = RunBatch(argc, argv);
  }

  t_main.TimeSectionEnd();
  SystemWideEventPublisher().PublishEvent(opensn::Event("ProgramExecuted"));

  return error_code;
}

void
ConsoleApp::ParseArguments(int argc, char** argv)
{
  bool input_file_found = false;
  for (int i = 1; i < argc; i++)
  {
    std::string argument(argv[i]);

    Log().Log() << "Parsing argument " << i << " " << argument;

    if (argument.find("-h") != std::string::npos or argument.find("--help") != std::string::npos)
    {
      Log().Log() << command_line_help_string;
      termination_posted_ = true;
    }
    else if (argument.find("--supress_beg_end_timelog") != std::string::npos)
    {
      supress_beg_end_timelog_ = true;
    }
    else if (argument.find("--allow_petsc_error_handler") != std::string::npos)
    {
      allow_petsc_error_handler_ = true;
    }
    else if (argument.find("--suppress_color") != std::string::npos) { suppress_color_ = true; }
    else if (argument.find("--dump-object-registry") != std::string::npos)
    {
      dump_registry_ = true;
      termination_posted_ = true;
    }
    // No-graphics option
    else if (argument.find("-b") != std::string::npos) { sim_option_interactive_ = false; }
    // Verbosity
    else if (argument.find("-v") != std::string::npos)
    {
      if ((i + 1) >= argc)
      {
        std::cerr << "Invalid option used with command line argument -v. Options are 0,1 or 2."
                  << std::endl;
        opensn::App::Exit(EXIT_FAILURE);
      }
      else
      {
        std::string v_option(argv[i + 1]);
        try
        {
          int level = std::stoi(v_option);
          Log().SetVerbosity(level);
          i++;
        }
        catch (const std::invalid_argument& e)
        {
          std::cerr << "Invalid option used with command line argument -v. Options are 0,1 or 2."
                    << std::endl;
          opensn::App::Exit(EXIT_FAILURE);
        }
      }
    }
    else if ((argument.find('=') == std::string::npos) and (!input_file_found))
    {
      input_file_name_ = argument;
      input_file_found = true;
      sim_option_interactive_ = false;
    } // no =
    else if (argument.find('=') != std::string::npos)
    {
      console_.GetCommandBuffer().push_back(argument);
    }
  }

  if (dump_registry_)
  {
    // FIXME
    // opensn::Registry::Instance().Dump(Log().Log(), Log().GetVerbosity());
    console_.DumpRegister();
  }
}

void
ConsoleApp::PrintHeader(const std::string& mode, int n_args)
{
  if (not supress_beg_end_timelog_)
  {
    Log().Log() << chi::Timer::GetLocalDateTimeString() << " Running openSn in " << mode
                << " mode with " << ProcessCount() << " processes.";
    Log().Log() << "openSn version " << GetVersionStr();
  }
  Log().Log() << "- number of arguments supplied: " << n_args;
}

int
ConsoleApp::RunInteractive(int argc, char** argv)
{
  PrintHeader("interactive", argc - 1);

  Log().LogAll();

  console_.FlushConsole();

  if (not input_file_name_.empty())
  {
    try
    {
      console_.ExecuteFile(input_file_name_, argc, argv);
    }
    catch (const std::exception& excp)
    {
      Log().LogAllError() << excp.what();
      // No quitting if file execution fails
    }
  }

  console_.RunConsoleLoop();

  if (not supress_beg_end_timelog_)
  {
    Log().Log() << "Final program time " << ProgramTimer().GetTimeString();
    Log().Log() << chi::Timer::GetLocalDateTimeString() << " execution finished.";
  }

  return 0;
}

int
ConsoleApp::RunBatch(int argc, char** argv)
{
  PrintHeader("batch", argc - 1);

  if (argc <= 1) Log().Log() << command_line_help_string;
  console_.FlushConsole();

#ifndef NDEBUG
  Log().Log() << "Waiting...";
  if (LocationID() == 0)
    for (int k = 0; k < 2; ++k)
    {
      usleep(1000000);
      Log().Log() << k;
    }

  Barrier();
#endif

  int error_code = 0;

  if ((not input_file_name_.empty()) and (not termination_posted_))
  {
    try
    {
      error_code = console_.ExecuteFile(input_file_name_, argc, argv);
    }
    catch (const std::exception& excp)
    {
      Log().LogAllError() << excp.what();
      opensn::App::Exit(EXIT_FAILURE);
    }
  }

  if (not supress_beg_end_timelog_)
  {
    Log().Log() << "\nFinal program time " << ProgramTimer().GetTimeString();
    Log().Log() << chi::Timer::GetLocalDateTimeString() << " Finished execution of "
                << input_file_name_;
  }

  return error_code;
}

std::string
ConsoleApp::GetVersionStr()
{
  return PROJECT_VERSION;
}
