/** @file Runtime file*/
#include "lua/framework/runtime.h"
#include "config.h"
#include "framework/app.h"
#include "lua/base/console.h"
#include "framework/math/math.h"
#include "framework/mesh/mesh_handler/mesh_handler.h"
#include "framework/physics/physics_namespace.h"
#include "framework/post_processors/post_processor.h"
#include "framework/event_system/system_wide_event_publisher.h"
#include "framework/event_system/event.h"
#include "framework/object_factory.h"
#include "framework/mpi/mpi.h"
#include "framework/logging/log.h"
#include "framework/utils/timer.h"

#include <iostream>

#ifndef NDEBUG
#include <unistd.h>
#endif

#if defined(__MACH__)
#include <mach/mach.h>
#else
#include <unistd.h>
#endif

// Global variables
chi::Console& Chi::console = chi::Console::GetInstance();
chi::MPI_Info& Chi::mpi = chi::MPI_Info::GetInstance();
chi::Timer Chi::program_timer;

std::vector<chi_mesh::MeshHandlerPtr> Chi::meshhandler_stack;
int Chi::current_mesh_handler = -1;

std::vector<chi_mesh::SurfaceMeshPtr> Chi::surface_mesh_stack;
std::vector<chi_mesh::FFInterpPtr> Chi::field_func_interpolation_stack;
std::vector<chi_mesh::UnpartMeshPtr> Chi::unpartitionedmesh_stack;

std::vector<chi_physics::MaterialPtr> Chi::material_stack;
std::vector<chi_physics::MultiGroupXSPtr> Chi::multigroup_xs_stack;
std::vector<chi_physics::FieldFunctionPtr> Chi::field_function_stack;

std::vector<chi_math::AngularQuadraturePtr> Chi::angular_quadrature_stack;

std::vector<ChiObjectPtr> Chi::object_stack;
std::vector<chi_math::SpatialDiscretizationPtr> Chi::sdm_stack;
std::vector<chi::PostProcessorPtr> Chi::postprocessor_stack;

// run_time quantities
bool Chi::run_time::termination_posted_ = false;
std::string Chi::run_time::input_file_name_;
bool Chi::run_time::sim_option_interactive_ = true;
bool Chi::run_time::allow_petsc_error_handler_ = false;
bool Chi::run_time::supress_beg_end_timelog_ = false;
bool Chi::run_time::suppress_color_ = false;
bool Chi::run_time::dump_registry_ = false;

const std::string Chi::run_time::command_line_help_string_ =
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

void
Chi::run_time::ParseArguments(int argc, char** argv)
{
  bool input_file_found = false;
  for (int i = 1; i < argc; i++)
  {
    std::string argument(argv[i]);

    // FIXME
    // Chi::log.Log() << "Parsing argument " << i << " " << argument;

    if (argument.find("-h") != std::string::npos or argument.find("--help") != std::string::npos)
    {
      // FIXME
      // Chi::log.Log() << Chi::run_time::command_line_help_string_;
      Chi::run_time::termination_posted_ = true;
    }
    else if (argument.find("--supress_beg_end_timelog") != std::string::npos)
    {
      Chi::run_time::supress_beg_end_timelog_ = true;
    }
    else if (argument.find("--allow_petsc_error_handler") != std::string::npos)
    {
      Chi::run_time::allow_petsc_error_handler_ = true;
    }
    else if (argument.find("--suppress_color") != std::string::npos)
    {
      Chi::run_time::suppress_color_ = true;
    }
    else if (argument.find("--dump-object-registry") != std::string::npos)
    {
      Chi::run_time::dump_registry_ = true;
      Chi::run_time::termination_posted_ = true;
    }
    // No-graphics option
    else if (argument.find("-b") != std::string::npos)
    {
      Chi::run_time::sim_option_interactive_ = false;
    } //-b
    // Verbosity
    else if (argument.find("-v") != std::string::npos)
    {
      if ((i + 1) >= argc)
      {
        std::cerr << "Invalid option used with command line argument "
                     "-v. Options are 0,1 or 2."
                  << std::endl;
        opensn::App::Exit(EXIT_FAILURE);
      }
      else
      {
        std::string v_option(argv[i + 1]);
        try
        {
          int level = std::stoi(v_option);
          // FIXME
          // Chi::log.SetVerbosity(level);
        }
        catch (const std::invalid_argument& e)
        {
          std::cerr << "Invalid option used with command line argument "
                       "-v. Options are 0,1 or 2."
                    << std::endl;
          opensn::App::Exit(EXIT_FAILURE);
        }
      }

    } //-v
    else if ((argument.find('=') == std::string::npos) and (!input_file_found))
    {
      Chi::run_time::input_file_name_ = argument;
      input_file_found = true;
      Chi::run_time::sim_option_interactive_ = false;
    } // no =
    else if (argument.find('=') != std::string::npos)
    {
      Chi::console.GetCommandBuffer().push_back(argument);
    }
  }

  if (Chi::run_time::dump_registry_)
  {
    // FIXME
    // ChiObjectFactory::GetInstance().DumpRegister();
    // Chi::console.DumpRegister();
  }
}

int
Chi::Initialize(int argc, char** argv, MPI_Comm communicator)
{
  int location_id = 0, number_processes = 1;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(communicator, &location_id);
  MPI_Comm_size(communicator, &number_processes);

  mpi.SetCommunicator(communicator);
  mpi.SetLocationID(location_id);
  mpi.SetProcessCount(number_processes);

  Chi::console.PostMPIInfo(location_id, number_processes);

  run_time::ParseArguments(argc, argv);

  run_time::InitPetSc(argc, argv);

  // FIXME
  // auto& t_main = Chi::log.CreateTimingBlock("ChiTech");
  // t_main.TimeSectionBegin();
  // chi::SystemWideEventPublisher::GetInstance().PublishEvent(chi::Event("ProgramStart"));

  return 0;
}

int
Chi::run_time::InitPetSc(int argc, char** argv)
{
  PETSC_COMM_WORLD = mpi.comm;
  PetscOptionsInsertString(nullptr, "-error_output_stderr");
  if (not Chi::run_time::allow_petsc_error_handler_)
    PetscOptionsInsertString(nullptr, "-no_signal_handler");

  PetscCall(PetscInitialize(&argc, &argv, nullptr, nullptr));

  return 0;
}

void
Chi::Finalize()
{
  // FIXME
  // auto& t_main = Chi::log.GetTimingBlock("ChiTech");
  // t_main.TimeSectionEnd();
  // chi::SystemWideEventPublisher::GetInstance().PublishEvent(chi::Event("ProgramExecuted"));
  meshhandler_stack.clear();

  surface_mesh_stack.clear();
  object_stack.clear();
  field_func_interpolation_stack.clear();
  unpartitionedmesh_stack.clear();

  object_stack.clear();
  material_stack.clear();
  multigroup_xs_stack.clear();

  PetscFinalize();
  MPI_Finalize();
}

int
Chi::RunInteractive(int argc, char** argv)
{
  if (not Chi::run_time::supress_beg_end_timelog_)
  {
    // FIXME
    // Chi::log.Log() << chi::Timer::GetLocalDateTimeString()
    //                << " Running ChiTech in interactive-mode with " << Chi::mpi.process_count
    //                << " processes.";
    //
    // Chi::log.Log() << "ChiTech version " << GetVersionStr();
  }

  // FIXME
  // Chi::log.Log() << "ChiTech number of arguments supplied: " << argc - 1;
  //
  // Chi::log.LogAll();

  Chi::console.FlushConsole();

  const auto& input_fname = Chi::run_time::input_file_name_;

  if (not input_fname.empty())
  {
    try
    {
      Chi::console.ExecuteFile(input_fname, argc, argv);
    }
    catch (const std::exception& excp)
    {
      // FIXME
      // Chi::log.LogAllError() << excp.what();
      // No quitting if file execution fails
    }
  }

  Chi::console.RunConsoleLoop();

  if (not Chi::run_time::supress_beg_end_timelog_)
  {
    // FIXME
    // Chi::log.Log() << "Final program time " << program_timer.GetTimeString();
    // Chi::log.Log() << chi::Timer::GetLocalDateTimeString() << " ChiTech finished execution.";
  }

  return 0;
}

int
Chi::RunBatch(int argc, char** argv)
{
  if (not Chi::run_time::supress_beg_end_timelog_)
  {
    // FIXME
    // Chi::log.Log() << chi::Timer::GetLocalDateTimeString() << " Running ChiTech in batch-mode
    // with "
    //                << Chi::mpi.process_count << " processes.";
    //
    // Chi::log.Log() << "ChiTech version " << GetVersionStr();
  }

  // FIXME
  // Chi::log.Log() << "ChiTech number of arguments supplied: " << argc - 1;

  // FIXME
  // if (argc <= 1) Chi::log.Log() << Chi::run_time::command_line_help_string_;
  Chi::console.FlushConsole();

#ifndef NDEBUG
  // FIXME
  // Chi::log.Log() << "Waiting...";
  if (Chi::mpi.location_id == 0)
    for (int k = 0; k < 2; ++k)
    {
      usleep(1000000);
      // Chi::log.Log() << k;
    }

  mpi.Barrier();
#endif

  const auto& input_fname = Chi::run_time::input_file_name_;
  int error_code = 0;

  if ((not input_fname.empty()) and (not Chi::run_time::termination_posted_))
  {
    try
    {
      error_code = Chi::console.ExecuteFile(input_fname, argc, argv);
    }
    catch (const std::exception& excp)
    {
      // FIXME
      // Chi::log.LogAllError() << excp.what();
      opensn::App::Exit(EXIT_FAILURE);
    }
  }

  if (not Chi::run_time::supress_beg_end_timelog_)
  {
    // FIXME
    // Chi::log.Log() << "\nFinal program time " << program_timer.GetTimeString();
    // Chi::log.Log() << chi::Timer::GetLocalDateTimeString() << " ChiTech finished execution of "
    //                << Chi::run_time::input_file_name_;
  }

  return error_code;
}

std::string
Chi::GetVersionStr()
{
  return PROJECT_VERSION;
}

chi::RegistryStatuses
Chi::GetStatusOfRegistries()
{
  chi::RegistryStatuses stats;

  // FIXME
  // const auto& object_factory = ChiObjectFactory::GetInstance();
  // for (const auto& [key, _] : object_factory.Registry())
  //   stats.objfactory_keys_.push_back(key);
  //
  // for (const auto& [key, _] : console.GetLuaFunctionRegistry())
  //   stats.console_lua_func_keys_.push_back(key);
  //
  // for (const auto& [key, _] : console.GetFunctionWrapperRegistry())
  //   stats.console_lua_wrapper_keys_.push_back(key);

  return stats;
}

chi::CSTMemory
Chi::GetMemoryUsage()
{
  double mem = 0.0;
#if defined(__MACH__)
  struct mach_task_basic_info info;
  mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
  long long int bytes;
  if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count) != KERN_SUCCESS)
  {
    bytes = 0;
  }
  bytes = info.resident_size;
  mem = (double)bytes;
#else
  long long int llmem = 0;
  long long int rss = 0;

  std::string ignore;
  std::ifstream ifs("/proc/self/stat", std::ios_base::in);
  ifs >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >>
    ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >>
    ignore >> ignore >> ignore >> ignore >> llmem >> rss;

  long long int page_size_bytes = sysconf(_SC_PAGE_SIZE);
  mem = rss * page_size_bytes;
  /*
  FILE* fp = NULL;
  if((fp = fopen( "/proc/self/statm", "r" )) == NULL)
    return 0;
  if(fscanf(fp, "%*s%*s%*s%*s%*s%lld", &llmem) != 1)
  {
    fclose(fp);
    return 0;
  }
  fclose(fp);*/

  // mem = llmem * (long long int)sysconf(_SC_PAGESIZE);
#endif

  chi::CSTMemory mem_struct(mem);

  return mem_struct;
}

double
Chi::GetMemoryUsageInMB()
{
  chi::CSTMemory mem_struct = GetMemoryUsage();

  return mem_struct.memory_mbytes;
}
