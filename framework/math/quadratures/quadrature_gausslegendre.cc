#include "framework/math/quadratures/quadrature_gausslegendre.h"
#include "framework/math/quadratures/legendre_poly/legendrepoly.h"
#include "framework/object_factory.h"
#include "framework/logging/log.h"
#include <cmath>
#include <algorithm>

#define uint unsigned int
#define scint static_cast<int>

namespace chi_math
{

RegisterChiObject(chi_math, QuadratureGaussLegendre);

chi::InputParameters
QuadratureGaussLegendre::GetInputParameters()
{
  chi::InputParameters params = Quadrature::GetInputParameters();

  // clang-format off
  params.SetGeneralDescription("General Gauss-Legendre quadrature");
  params.SetDocGroup("LuaQuadrature");
  // clang-format on

  params.ChangeExistingParamToOptional("order", 0);
  params.AddOptionalParameter(
    "max_root_finding_iters", 1000, "Maximum number of iterations used during root finding");
  params.AddOptionalParameter("root_finding_tol", 1.0e-12, "Root finding iterative tolerance");

  params.AddOptionalParameter("N", 1, "Number of quadrature points.");

  return params;
}

QuadratureGaussLegendre::QuadratureGaussLegendre(opensn::App& app,
                                                 const chi::InputParameters& params)
  : chi_math::Quadrature(app, params)
{
  const auto& assigned_params = params.ParametersAtAssignment();

  const int param_count = int(assigned_params.Has("order")) + int(assigned_params.Has("N"));
  ChiInvalidArgumentIf(param_count == 2, "Either \"order\" or \"N\" must be specified, not both");

  const uint max_iters = params.GetParamValue<uint>("max_root_finding_iters");
  const double tol = params.GetParamValue<double>("root_finding_tol");

  if (assigned_params.Has("order"))
  {
    const uint N = std::ceil(((int)order_ + 1) / 2.0);
    Initialize(N, verbose_, max_iters, tol);
  }
  else
  {
    const uint N = assigned_params.GetParamValue<uint>("N");
    order_ = static_cast<QuadratureOrder>(std::min(scint(2 * N + 1), 43));
    Initialize(N, verbose_, max_iters, tol);
  }
}

QuadratureGaussLegendre::QuadratureGaussLegendre(
  opensn::App& app, QuadratureOrder in_order, bool verbose, unsigned int max_iters, double tol)
  : chi_math::Quadrature(app, in_order)
{
  const unsigned int N = std::ceil(((int)order_ + 1) / 2.0);
  Initialize(N, verbose, max_iters, tol);
}

QuadratureGaussLegendre::QuadratureGaussLegendre(
  opensn::App& app, unsigned int N, bool verbose, unsigned int max_iters, double tol)
  : chi_math::Quadrature(app, (QuadratureOrder)(2 * N - 1))
{
  Initialize(N, verbose, max_iters, tol);
}

void
QuadratureGaussLegendre::Initialize(unsigned int N,
                                    bool verbose,
                                    unsigned int max_iters,
                                    double tol)
{
  switch (order_)
  {
    default:
    {
      // FIXME: make this work
      // if (verbose)
      //   Chi::log.Log() << "Initializing Gauss-Legendre Quadrature with " << N << " q-points";

      // Compute the roots
      auto roots = FindRoots(N, max_iters, tol);
      for (auto v : roots)
        qpoints_.emplace_back(v);

      // Compute the weights
      weights_.resize(N, 1.0);
      for (size_t k = 0; k < qpoints_.size(); k++)
      {
        weights_[k] =
          2.0 * (1.0 - qpoints_[k][0] * qpoints_[k][0]) /
          ((N + 1) * (N + 1) * Legendre(N + 1, qpoints_[k][0]) * Legendre(N + 1, qpoints_[k][0]));

        // FIXME: make this work
        // if (verbose)
        //   Chi::log.Log() << "root[" << k << "]=" << qpoints_[k][0] << ", weight=" << weights_[k];
      } // for abscissae

      break;
    }
  } // switch order

  range_ = {-1, +1};
}

std::vector<double>
QuadratureGaussLegendre::FindRoots(unsigned int N, unsigned int max_iters, double tol)
{
  // Populate init guess
  // This initial guess proved to be quite important
  // at higher N since the roots start to get
  // squeezed to -1 and 1.
  int num_search_intvls = 1000;
  if (N > 64) num_search_intvls *= 10;
  if (N > 256) num_search_intvls *= 10;
  if (N > 768) num_search_intvls *= 10;

  if (N > 2056)
  {
    num_search_intvls *= 10;
    // FIXME: make this work
    // Chi::log.Log0Warning() << "chi_math::QuadratureGaussLegendre::FindRoots: "
    //                        << "The order of the polynomial for which to find the roots is "
    //                        << "greater than 2056. Accuracy of the root finder will be diminished
    //                        "
    //                        << "along with a reduction in stability.";
  }

  // For this code we simply check to see where the
  // polynomial changes sign.
  double delta = 2.0 / num_search_intvls;
  std::vector<double> xk(N, 0.0);
  int counter = -1;
  for (size_t i = 0; i < num_search_intvls; i++)
  {
    double x_i = -1.0 + i * delta;
    double x_ip1 = x_i + delta;

    if (Legendre(N, x_i) * Legendre(N, x_ip1) < 0.0) xk[++counter] = (x_ip1 + x_i) / 2.0;
  }

  // Apply algorithm
  // Refer to equation 4.3 in [1]. Sum 1 (S1) is used in the
  // computation of B at x_k. Sum 2 (S2) is used in equation 4.3.
  // Equation 4.3 is broken up into pieces as follows:
  //  - a = block bracket containing the second derivative
  //  - b = denominator
  //  - c = everything but xold
  for (int k = 0; k < N; k++)
  {
    for (size_t iteration = 0; iteration < max_iters; iteration++)
    {
      double xold = xk[k];
      double f = Legendre(N, xold);        // Function evaluation
      double fp = dLegendredx(N, xold);    // First derivative
      double fpp = d2Legendredx2(N, xold); // Second derivative

      // Compute sum 1
      double S1 = 0.0;
      for (int i = 0; i <= (k - 1); i++)
        S1 += 1.0 / (xk[k] - xk[i]);

      // Compute B at x_k
      double B_xk = fp - f * S1;

      // Compute sum 2
      double S2 = 0.0;
      for (int i = 0; i <= (k - 1); i++)
        S2 += 1.0 / (xk[k] - xk[i]) / (xk[k] - xk[i]);

      // Compute final formula
      double a = fpp + f * S2;
      double b = B_xk * B_xk + fp * fp - f * a;
      double c = 2.0 * f * B_xk / b;

      xk[k] = xold - c;

      if (std::fabs(xk[k] - xold) < tol) break;
    } // for iteration
  }   // for k

  std::stable_sort(xk.begin(), xk.end());

  return xk;
}

} // namespace chi_math
