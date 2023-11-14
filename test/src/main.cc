#include "lua/framework/init.h"
#include "lua/base/console_app.h"

int
main(int argc, char** argv)
{
  MPI_Comm comm = MPI_COMM_WORLD;
  Init init(argc, argv);
  ConsoleApp app(comm);
  return app.Run(argc, argv);
}
