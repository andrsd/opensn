#include "framework/init.h"
#include "framework/runtime.h"
#include "framework/console/console.h"
#include "framework/logging/log.h"
#include "framework/event_system/system_wide_event_publisher.h"
#include "framework/event_system/event.h"
#include "lua/modules/modules_lua.h"

int
main(int argc, char** argv)
{
  Init init(argc, argv);

  chi_modules::lua_utils::LoadRegisteredLuaItems();
  Chi::console.PostMPIInfo(Chi::mpi.location_id, Chi::mpi.process_count);

  Chi::run_time::ParseArguments(argc, argv);

  Chi::run_time::InitPetSc(argc, argv);

  auto& t_main = Chi::log.CreateTimingBlock("ChiTech");
  t_main.TimeSectionBegin();
  chi::SystemWideEventPublisher::GetInstance().PublishEvent(chi::Event("ProgramStart"));

  int error_code;
  if (Chi::run_time::sim_option_interactive_) error_code = Chi::RunInteractive(argc, argv);
  else
    error_code = Chi::RunBatch(argc, argv);

  return error_code;
}
