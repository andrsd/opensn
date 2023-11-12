#include "lua/framework/init.h"
#include "framework/runtime.h"
#include "framework/console/console.h"

int
main(int argc, char** argv)
{
  Init init(argc, argv);

  int error_code;
  if (Chi::run_time::sim_option_interactive_) error_code = Chi::RunInteractive(argc, argv);
  else
    error_code = Chi::RunBatch(argc, argv);

  return error_code;
}
