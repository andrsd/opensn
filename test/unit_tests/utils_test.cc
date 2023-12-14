#include "gmock/gmock.h"
#include "framework/utils/utils.h"

using namespace opensn;
using namespace testing;

TEST(utils, print_iteration_progress)
{
  const unsigned int I = 4;
  const size_t N = 39;

  std::stringstream progress;
  for (size_t i = 0; i < N; ++i)
    progress << PrintIterationProgress(i, N, I);
  EXPECT_THAT(progress.str(), HasSubstr("25.00  50.00  75.00 100.00"));
}
