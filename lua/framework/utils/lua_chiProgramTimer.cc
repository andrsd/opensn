#include "console/chi_console.h"

#include "chi_lua.h"

#include "chi_runtime.h"
#include "../chi_timer.h"

namespace chi::lua_utils
{

int chiProgramTime(lua_State* L);

RegisterLuaFunctionAsIs(chiProgramTime);

/**Returns the program time as determined from the home location (involves a
 * collective broadcast).*/
int
chiProgramTime(lua_State* L)
{
  double time;
  if (Chi::mpi.location_id == 0) time = Chi::program_timer.GetTime() / 1000.0;

  MPI_Bcast(&time,          // send/recv buffer
            1,              // count
            MPI_DOUBLE,     // datatype
            0,              // root
            Chi::mpi.comm); // communicator

  lua_pushnumber(L, time);
  return 1;
}

} // namespace chi::lua_utils