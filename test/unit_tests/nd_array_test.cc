#include "gmock/gmock.h"
#include "framework/data_types/ndarray.h"
#include "framework/math/quadratures/angular_product_quadrature.h"
#include "framework/math/math_range.h"

using namespace opensn;
using namespace testing;

typedef NDArray<double> IJKArrayDbl;

TEST(NDAarray, Ctor)
{
  NDArray<double> nd_array(std::vector<size_t>{2, 2, 2});
  std::vector<double> vals(nd_array.begin(), nd_array.end());
  EXPECT_THAT(vals, ElementsAre(0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(NDAarray, CtorArray)
{
  NDArray<double> nd_array(std::array<size_t, 3>{2, 2, 2});
  std::vector<double> vals(nd_array.begin(), nd_array.end());
  EXPECT_THAT(vals, ElementsAre(0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(NDAarray, CtorInitList)
{
  NDArray<double> nd_array({2, 2, 2});
  std::vector<double> vals(nd_array.begin(), nd_array.end());
  EXPECT_THAT(vals, ElementsAre(0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(NDAarray, CtorStdVector)
{
  NDArray<double> nd_array(std::vector<size_t>{2, 2, 2}, 0.0);
  std::vector<double> vals(nd_array.begin(), nd_array.end());
  EXPECT_THAT(vals, ElementsAre(0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(NDAarray, CtorStdArray)
{
  NDArray<double> nd_array(std::array<size_t, 3>{2, 2, 2}, 0.0);
  std::vector<double> vals(nd_array.begin(), nd_array.end());
  EXPECT_THAT(vals, ElementsAre(0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(ndarray, CtorList)
{
  NDArray<double> nd_array({2, 2, 2}, 0.0);
  std::vector<double> vals(nd_array.begin(), nd_array.end());
  EXPECT_THAT(vals, ElementsAre(0, 0, 0, 0, 0, 0, 0, 0));
}

TEST(NDAarray, CtorNone)
{
  NDArray<double> nd_array;
  std::vector<double> vals(nd_array.begin(), nd_array.end());
  EXPECT_THAT(vals, ElementsAre());
}

TEST(NDAarray, IterConst)
{
  NDArray<double> nd_array(std::vector<size_t>{2, 2, 2});
  nd_array.set(1.0);
  std::vector<double> vals(nd_array.begin(), nd_array.end());
  EXPECT_THAT(vals, ElementsAre(1., 1., 1., 1., 1., 1., 1., 1.));
}

TEST(NDAarray, Size)
{
  NDArray<double> nd_array(std::array<size_t, 3>{2, 2, 2});
  nd_array.set(1.0);
  EXPECT_EQ(nd_array.size(), 8);
}

TEST(NDAarray, Empty)
{
  NDArray<double> nd_array1(std::array<size_t, 3>{2, 2, 2});
  EXPECT_FALSE(nd_array1.empty());

  NDArray<double> nd_array2;
  EXPECT_TRUE(nd_array2.empty());
}

TEST(NDAarray, Dimension)
{
  NDArray<double> nd_array(std::array<size_t, 3>{2, 4, 7});
  nd_array.set(1.0);
  EXPECT_THAT(nd_array.dimension(), ElementsAre(2, 4, 7));
}

TEST(NDAarray, Rank)
{
  NDArray<double> nd_array(std::array<size_t, 3>{2, 4, 7});
  EXPECT_EQ(nd_array.rank(), 3);
}

namespace
{

IJKArrayDbl
WDD_IJK_Sweep2(const std::array<size_t, 3>& mesh_divs,
               const std::array<double, 3>& mesh_lengths,
               const std::array<double, 6>& bcs,
               const IJKArrayDbl& sigma_t,
               const IJKArrayDbl& q,
               const AngularQuadrature& quad,
               bool verbose = false)
{
  const int Nx = static_cast<int>(mesh_divs[0]);
  const int Ny = static_cast<int>(mesh_divs[1]);
  const int Nz = static_cast<int>(mesh_divs[2]);

  const double dx = mesh_lengths[0] / Nx;
  const double dy = mesh_lengths[1] / Ny;
  const double dz = mesh_lengths[2] / Nz;

  IJKArrayDbl phi_0(mesh_divs);
  phi_0.set(0.0);

  auto iorder = opensn::Range<int>(0, Nx);
  auto jorder = opensn::Range<int>(0, Ny);
  auto korder = opensn::Range<int>(0, Nz);

  const auto& D2M = quad.GetDiscreteToMomentOperator();

  int n = 0;
  for (const auto& omega_n : quad.omegas_)
  {
    // Determine sweep ordering
    if (omega_n.x > 0.0) iorder = opensn::Range<int>(0, Nx);
    else
      iorder = opensn::Range<int>(Nx - 1, -1, -1);

    if (omega_n.y > 0.0) jorder = opensn::Range<int>(0, Ny);
    else
      jorder = opensn::Range<int>(Ny - 1, -1, -1);

    if (omega_n.z > 0.0) korder = opensn::Range<int>(0, Nz);
    else
      korder = opensn::Range<int>(Nz - 1, -1, -1);

    // Sweep cells
    IJKArrayDbl psi_ds_x(mesh_divs);
    IJKArrayDbl psi_ds_y(mesh_divs);
    IJKArrayDbl psi_ds_z(mesh_divs);
    for (auto k : korder)
      for (auto j : jorder)
        for (auto i : iorder)
        {
          double psi_us_x = (omega_n.x > 0.0) ? bcs[0] : bcs[1];
          double psi_us_y = (omega_n.y > 0.0) ? bcs[2] : bcs[3];
          double psi_us_z = (omega_n.z > 0.0) ? bcs[4] : bcs[5];

          if (omega_n.x > 0.0 and i > 0) psi_us_x = psi_ds_x(i - 1, j, k);
          if (omega_n.x < 0.0 and i < (Nx - 1)) psi_us_x = psi_ds_x(i + 1, j, k);

          if (omega_n.y > 0.0 and j > 0) psi_us_y = psi_ds_y(i, j - 1, k);
          if (omega_n.y < 0.0 and j < (Ny - 1)) psi_us_y = psi_ds_y(i, j + 1, k);

          if (omega_n.z > 0.0 and k > 0) psi_us_z = psi_ds_z(i, j, k - 1);
          if (omega_n.z < 0.0 and k < (Nz - 1)) psi_us_z = psi_ds_z(i, j, k + 1);

          double rhs = q(i, j, k) / (4.0 * M_PI);
          if (Nx > 1) rhs += 2.0 * std::fabs(omega_n.x) * psi_us_x / dx;
          if (Ny > 1) rhs += 2.0 * std::fabs(omega_n.y) * psi_us_y / dy;
          if (Nz > 1) rhs += 2.0 * std::fabs(omega_n.z) * psi_us_z / dz;

          double lhs = sigma_t(i, j, k);
          if (Nx > 1) lhs += 2.0 * std::fabs(omega_n.x) / dx;
          if (Ny > 1) lhs += 2.0 * std::fabs(omega_n.y) / dy;
          if (Nz > 1) lhs += 2.0 * std::fabs(omega_n.z) / dz;

          double psi_ijk = rhs / lhs;

          phi_0(i, j, k) += D2M[0][n] * psi_ijk;

          psi_ds_x(i, j, k) = 2.0 * psi_ijk - psi_us_x;
          psi_ds_y(i, j, k) = 2.0 * psi_ijk - psi_us_y;
          psi_ds_z(i, j, k) = 2.0 * psi_ijk - psi_us_z;
        }
    ++n;
  } // for omega

  return phi_0;
}

} // namespace

TEST(NDAarray, WDD_IJK_Sweep)
{
  mpi::Environment env;
  bool verbose = false;

  const std::array<size_t, 3> mesh_divisions = {1, 1, 10};
  const std::array<double, 3> mesh_lengths = {1.0, 1.0, 10.0};
  const std::array<double, 6> bcs = {0.0, 0.0, 0.0, 0.0, 0.5, 0.0};

  IJKArrayDbl sigma_t(mesh_divisions, 0.2);
  IJKArrayDbl q(mesh_divisions, 0.0);

  auto pquad = std::make_shared<AngularQuadratureProdGL>(1, verbose);

  pquad->BuildDiscreteToMomentOperator(0, 1);
  auto phi = WDD_IJK_Sweep2(mesh_divisions, mesh_lengths, bcs, sigma_t, q, *pquad, verbose);

  std::vector<double> vals(phi.begin(), phi.end());
  EXPECT_THAT(vals,
              ElementsAre(DoubleNear(0.426183, 1e-6),
                          DoubleNear(0.300345, 1e-6),
                          DoubleNear(0.211662, 1e-6),
                          DoubleNear(0.149165, 1e-6),
                          DoubleNear(0.105121, 1e-6),
                          DoubleNear(0.0740825, 1e-7),
                          DoubleNear(0.0522083, 1e-7),
                          DoubleNear(0.0367928, 1e-7),
                          DoubleNear(0.0259291, 1e-7),
                          DoubleNear(0.018273, 1e-6)));
}
