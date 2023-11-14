#pragma once

namespace opensn
{

/**
 * Structure for memory usage.
 */
struct MemoryInfo
{
  double memory_bytes = 0.0;
  double memory_kbytes = 0.0;
  double memory_mbytes = 0.0;
  double memory_gbytes = 0.0;

  MemoryInfo() = default;

  explicit MemoryInfo(double in_mem);

  MemoryInfo& operator=(const MemoryInfo& in_struct) = default;
};

/**
 * Get current memory usage.
 */
static MemoryInfo GetMemoryUsage();

/**
 * Get current memory usage in Megabytes.
 */
static double GetMemoryUsageInMB();

} // namespace opensn
