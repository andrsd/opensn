#pragma once

#include <vector>

#include "framework/mesh/mesh.h"

namespace opensn
{
struct QuadraturePointPhiTheta;

enum class AngularQuadratureType
{
  Arbitrary = 1,
  ProductQuadrature = 2,
  SLDFESQ = 3
};

/**Simple structure to add names to the angle components.*/
struct QuadraturePointPhiTheta
{
  double phi = 0.0;
  double theta = 0.0;
  QuadraturePointPhiTheta(const double phi, const double theta) : phi(phi), theta(theta) {}
};

/**Base class for angular quadratures.*/
class AngularQuadrature
{
public:
  const AngularQuadratureType type_;

public:
  std::vector<QuadraturePointPhiTheta> abscissae_;
  std::vector<double> weights_;
  std::vector<Vector3> omegas_;

  struct HarmonicIndices
  {
    unsigned int ell = 0;
    int m = 0;

    HarmonicIndices() = default;
    HarmonicIndices(unsigned int in_ell, int in_m) : ell(in_ell), m(in_m) {}

    bool operator==(const HarmonicIndices& other) const
    {
      return (ell == other.ell and m == other.m);
    }
  };

protected:
  std::vector<std::vector<double>> d2m_op_;
  std::vector<std::vector<double>> m2d_op_;
  std::vector<HarmonicIndices> m_to_ell_em_map_;
  bool d2m_op_built_ = false;
  bool m2d_op_built_ = false;

public:
  AngularQuadrature() : type_(AngularQuadratureType::Arbitrary) {}

  explicit AngularQuadrature(AngularQuadratureType in_type) : type_(in_type) {}

  virtual ~AngularQuadrature() = default;

  /**Optimizes the angular quadrature for polar symmetry by removing
   * all the direction with downward pointing polar angles.
   *
   * \param normalization float. (Optional) The default is a negative number
   *                             which does not apply any normalization. If a
   *                             positive number is provided, the weights will be
   *                             normalized to sum to this number.*/
  virtual void OptimizeForPolarSymmetry(double normalization);

protected:
  /**Populates a map of moment m to the Spherical Harmonic indices
   * required.*/
  virtual void MakeHarmonicIndices(unsigned int scattering_order, int dimension);

public:
  /**Computes the discrete to moment operator.*/
  virtual void BuildDiscreteToMomentOperator(unsigned int scattering_order, int dimension);
  /**Computes the moment to discrete operator.*/
  virtual void BuildMomentToDiscreteOperator(unsigned int scattering_order, int dimension);

  /**Returns a reference to the precomputed d2m operator. This will
   * throw a std::logic_error if the operator has not been built yet.
   * The operator is accessed as [m][d], where m is the moment index
   * and d is the direction index.*/
  std::vector<std::vector<double>> const& GetDiscreteToMomentOperator() const;

  /**Returns a reference to the precomputed m2d operator. This will
   * throw a std::logic_error if the operator has not been built yet.
   * The operator is accessed as [m][d], where m is the moment index
   * and d is the direction index.*/
  std::vector<std::vector<double>> const& GetMomentToDiscreteOperator() const;

  /**Returns a reference to the precomputed harmonic index map. This will
   * throw a std::logic_error if the map has not been built yet.*/
  const std::vector<HarmonicIndices>& GetMomentToHarmonicsIndexMap() const;
};

class AngularQuadratureCustom : public AngularQuadrature
{
public:
  /**Constructor using custom directions.*/
  AngularQuadratureCustom(std::vector<double>& azimuthal,
                          std::vector<double>& polar,
                          std::vector<double>& in_weights,
                          bool verbose);
};

} // namespace opensn
