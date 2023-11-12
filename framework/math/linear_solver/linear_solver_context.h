#pragma once

#include <petscksp.h>

namespace opensn
{
class App;
}

namespace chi_math
{

enum class ResidualScaleType
{
  NONE = 0,
  RHS_NORM = 1,
  RHS_PRECONDITIONED_NORM = 2,
  CUSTOM_SCALE = 3
};

struct LinearSolverContext
{
private:
  opensn::App& app_;

public:
  double rhs_norm = 0.0;
  double rhs_preconditioned_norm = 0.0;
  double custom_residual_scale = 1.0;
  ResidualScaleType residual_scale_type = ResidualScaleType::NONE;

  explicit LinearSolverContext(opensn::App& app) : app_(app) {}

  virtual int MatrixAction(Mat& matrix, Vec& vector, Vec& action) { return 0; }

  virtual ~LinearSolverContext() = default;

  opensn::App& App() const { return app_; }
};

} // namespace chi_math
