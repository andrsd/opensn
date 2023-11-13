#include "lua/base/lua.h"
#include "framework/runtime.h"
#include "mpi_lua.h"
#include "lua/base/console.h"

namespace chi_mpi_utils
{

RegisterLuaFunctionAsIs(chiMPIBarrier);

int
chiMPIBarrier(lua_State* L)
{
  // MPI_Barrier(Chi::mpi.comm);
  return 0;
}

} // namespace chi_mpi_utils
