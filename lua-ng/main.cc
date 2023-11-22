#include "lua-ng/lua_app.h"
#include "lua-ng/console.h"
#include "lua-ng/module.h"

int
main(int argc, char** argv)
{
  MPI_Init(&argc, &argv);

  opensnluang::LuaApp app(MPI_COMM_WORLD);
  auto console = opensnluang::Console::GetInstance();
  opensnluang::Bind(console.GetConsoleState());
  int error_code = app.Run(argc, argv);

  MPI_Finalize();
  return error_code;
}
