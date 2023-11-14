#include "memory_info.h"
#if defined(__MACH__)
#include <mach/mach.h>
#else
#include <unistd.h>
#endif

namespace opensn
{

MemoryInfo::MemoryInfo(double in_mem)
{
  memory_bytes = in_mem;
  memory_kbytes = in_mem / 1024.0;
  memory_mbytes = in_mem / 1024.0 / 1024.0;
  memory_gbytes = in_mem / 1024.0 / 1024.0 / 1024.0;
}

MemoryInfo
GetMemoryUsage()
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
#endif

  return MemoryInfo(mem);
}

double
GetMemoryUsageInMB()
{
  MemoryInfo info = GetMemoryUsage();

  return info.memory_mbytes;
}

} // namespace opensn
