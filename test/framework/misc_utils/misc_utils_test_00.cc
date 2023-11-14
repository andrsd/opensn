#include "framework/utils/utils.h"
#include "lua/framework/runtime.h"
#include "framework/logging/log.h"
#include "lua/base/console.h"

namespace chi_unit_tests
{

chi::ParameterBlock chi_misc_utils_Test00(const chi::InputParameters& params);

RegisterWrapperFunction(chi_unit_tests, chi_misc_utils_Test00, nullptr, chi_misc_utils_Test00);

chi::ParameterBlock
chi_misc_utils_Test00(const chi::InputParameters&)
{
  // FIXME
  // Chi::log.Log() << "GOLD_BEGIN";
  // Chi::log.Log() << "Testing chi_misc_utils::PrintIterationProgress\n";

  const unsigned int I = 4;
  const size_t N = 39;

  std::stringstream progress;
  for (size_t i = 0; i < N; ++i)
  {
    progress << chi::PrintIterationProgress(i, N, I);
  }

  // Chi::log.Log() << progress.str();

  // Chi::log.Log() << "GOLD_END";
  return chi::ParameterBlock();
}

} // namespace chi_unit_tests
