#include "init.h"
#include "framework/runtime.h"
#include "framework/console/console.h"
#include "framework/logging/log.h"
#include "lua/modules/modules_lua.h"
#include "framework/event_system/event.h"
#include "framework/event_system/system_wide_event_publisher.h"

Init::Init(int argc, char** argv) : opensn::Init(argc, argv)
{
  chi_modules::lua_utils::LoadRegisteredLuaItems();
  Chi::console.PostMPIInfo(Chi::mpi.location_id, Chi::mpi.process_count);

  Chi::run_time::ParseArguments(argc, argv);

  auto& t_main = Chi::log.CreateTimingBlock("ChiTech");
  t_main.TimeSectionBegin();
  chi::SystemWideEventPublisher::GetInstance().PublishEvent(chi::Event("ProgramStart"));
}
