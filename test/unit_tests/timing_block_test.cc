#include "gmock/gmock.h"
#include "framework/runtime.h"
#include "framework/logging/log.h"
#include "framework/utils/timer.h"

using namespace opensn;
using namespace testing;

TEST(TimingBlock, Test)
{
  mpi::Environment env;
  Initialize();

  auto& t_main = opensn::log.CreateTimingBlock("Timing_Main");
  t_main.TimeSectionBegin();
  {
    // Some overhead
    opensn::Sleep(std::chrono::milliseconds(200));

    auto& t_1 = opensn::log.CreateOrGetTimingBlock("Part1", "Timing_Main");
    t_1.TimeSectionBegin();
    opensn::Sleep(std::chrono::milliseconds(300));
    t_1.TimeSectionEnd();

    auto& t_2 = opensn::log.CreateOrGetTimingBlock("Part2", "Timing_Main");
    t_2.TimeSectionBegin();
    opensn::Sleep(std::chrono::milliseconds(333));
    t_2.TimeSectionEnd();

    auto& t_3 = opensn::log.GetTimingBlock("Part2");
    t_3.TimeSectionBegin();
    opensn::Sleep(std::chrono::milliseconds(123));
    t_3.TimeSectionEnd();
  }
  t_main.TimeSectionEnd();

  auto out = opensn::log.GetTimingBlock(opensn::name).MakeGraphString();
  // split output into lines
  std::vector<std::string> lines;
  std::istringstream istr(out);
  while (!istr.eof())
  {
    std::string s;
    getline(istr, s);
    lines.push_back(s);
  }
  EXPECT_THAT(lines[3], MatchesRegex(".+(openSn).+(1).+(0\\.9).+(0\\.9).+(--).+"));
  EXPECT_THAT(lines[4], MatchesRegex(".+(Timing_Main).+(1).+(0\\.9).+(0\\.9).+(100\\.00%).+"));
  EXPECT_THAT(lines[5], MatchesRegex(".+(Part1).+(1).+(0\\.3).+(0\\.3).+(31\\.).+%.+"));
  EXPECT_THAT(lines[6], MatchesRegex(".+(Part2).+(2).+(0\\.4).+(0\\.2).+(47\\.).+%.+"));

  Finalize();
}
