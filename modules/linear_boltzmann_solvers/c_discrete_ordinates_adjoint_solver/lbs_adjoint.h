#pragma once

#include <array>

namespace opensn
{
class App;
}

namespace lbs
{
void TestFunction();

std::array<double, 2>
MakeExpRepFromP1(opensn::App& app, const std::array<double, 4>& P1_moments, bool verbose = false);
} // namespace lbs
