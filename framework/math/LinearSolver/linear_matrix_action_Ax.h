#pragma once

#include "opensn/framework/math/LinearSolver/linear_solver_context.h"

namespace chi_math
{
template <class MatType, class VecType>
int LinearSolverMatrixAction(MatType matrix, VecType vector, VecType action);
} // namespace chi_math
