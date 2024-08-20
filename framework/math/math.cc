// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#include "framework/math/math.h"
#include <assert.h>

namespace opensn
{

double
Factorial(const int x)
{
  double factorial_value = 1.0;
  for (int i = 2; i <= x; ++i)
    factorial_value *= i;

  return factorial_value;
}

std::pair<double, double>
OmegaToPhiThetaSafe(const Vector3& omega)
{
  // Notes: asin maps [-1,+1] to [-pi/2,+pi/2]
  //        acos maps [-1,+1] to [0,pi]
  // This mapping requires some logic for determining the azimuthal angle.
  //
  const auto omega_hat = omega.Normalized();

  double mu = omega_hat.z;
  mu = std::min(mu, 1.0);
  mu = std::max(mu, -1.0);

  double theta = acos(mu);

  // Handling omega aligned to k_hat
  if (std::fabs(omega_hat.z) < 1.0e-16)
    return {0.0, theta};

  double cos_phi = omega_hat.x / sin(theta);
  cos_phi = std::min(cos_phi, 1.0);
  cos_phi = std::max(cos_phi, -1.0);

  // Computing varphi for NE and NW quadrant
  if (omega_hat.y >= 0.0)
    return {acos(cos_phi), theta};
  // Computing varphi for SE and SW quadrant
  else
    return {2.0 * M_PI - acos(cos_phi), theta};
}

void
PrintVector(const std::vector<double>& x)
{
  for (auto& xi : x)
    std::cout << xi << ' ';
  std::cout << std::endl;
}

void
Scale(std::vector<double>& x, const double& val)
{
  for (double& xi : x)
    xi *= val;
}

void
Set(std::vector<double>& x, const double& val)
{
  for (double& xi : x)
    xi = val;
}

double
Dot(const std::vector<double>& x, const std::vector<double>& y)
{
  // Error Checks
  assert(x.size() > 0);
  assert(y.size() > 0);
  assert(x.size() == y.size());
  // Local Variables
  size_t n = x.size();
  double val = 0.0;

  for (size_t i = 0; i != n; i++)
    val += x[i] * y[i];

  return val;
}

std::vector<double>
Mult(const std::vector<double>& x, const double& val)
{
  size_t n = x.size();
  std::vector<double> y(n);

  for (size_t i = 0; i != n; ++i)
    y[i] = val * x[i];

  return y;
}

double
Vec1Norm(const std::vector<double>& x)
{
  // Local Variables
  size_t n = x.size();
  double val = 0.0;

  for (size_t i = 0; i != n; i++)
    val += std::fabs(x[i]);

  return val;
}

double
Vec2Norm(const std::vector<double>& x)
{
  // Local Variables
  size_t n = x.size();
  double val = 0.0;

  for (size_t i = 0; i != n; i++)
    val += x[i] * x[i];

  return std::sqrt(val);
}

double
VecInfinityNorm(const std::vector<double>& x)
{
  // Local Variables
  size_t n = x.size();
  double val = 0.0;

  for (size_t i = 0; i != n; i++)
    val += std::max(std::fabs(x[i]), val);

  return val;
}

double
VecPNorm(const std::vector<double>& x, const double& p)
{
  // Local Variables
  size_t n = x.size();
  double val = 0.0;

  for (size_t i = 0; i != n; i++)
    val += std::pow(std::fabs(x[i]), p);

  return std::pow(val, 1.0 / p);
}

} // namespace opensn
