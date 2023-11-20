#pragma once

#include <map>
#include <vector>

#include "framework/math/quadratures/angular_quadrature_base.h"

namespace opensn
{

enum class ProductQuadratureType
{
  UNKNOWN = 0,
  GAUSS_LEGENDRE = 1,
  GAUSS_CHEBYSHEV = 2,
  GAUSS_LEGENDRE_LEGENDRE = 3,
  GAUSS_LEGENDRE_CHEBYSHEV = 4,
  CUSTOM_QUADRATURE = 5,
};

/** Class for product quadratures*/
class ProductQuadrature : public AngularQuadrature
{
public:
  std::vector<double> polar_ang_;
  std::vector<double> azimu_ang_;

protected:
  /** Linear indices of ordered directions mapped to polar level. */
  std::map<unsigned int, std::vector<unsigned int>> map_directions_;

protected:
  ProductQuadrature() : AngularQuadrature(AngularQuadratureType::ProductQuadrature) {}

public:
  ~ProductQuadrature() override = default;

  /**Initializes the quadrature with custom angles and weights.*/
  void AssembleCosines(const std::vector<double>& azimuthal,
                       const std::vector<double>& polar,
                       const std::vector<double>& in_weights,
                       bool verbose);

  /**Optimizes the angular quadrature for polar symmetry by removing
   * all the direction with downward pointing polar angles.
   *
   * \param normalization float. (Optional) The default is a negative number
   *                             which does not apply any normalization. If a
   *                             positive number is provided, the weights will be
   *                             normalized to sum to this number.*/
  void OptimizeForPolarSymmetry(double normalization) override;
  /**Obtains the abscissae index given the indices of the
   * polar angle index and the azimuthal angle index.*/
  unsigned int GetAngleNum(const unsigned int polar_angle_index,
                           const unsigned int azimu_angle_index) const
  {
    return map_directions_.at(polar_angle_index)[azimu_angle_index];
  }
  /** Return constant reference to map_directions. */
  const std::map<unsigned int, std::vector<unsigned int>>& GetDirectionMap() const
  {
    return map_directions_;
  }
};

class AngularQuadratureProdGL : public ProductQuadrature
{
public:
  /**Constructor for Angular Gauss-Legendre.*/
  explicit AngularQuadratureProdGL(int Np, bool verbose = false);
};

class AngularQuadratureProdGLL : public ProductQuadrature
{
public:
  /**Constructor for Angular Gauss-Legendre-Legendre.*/
  explicit AngularQuadratureProdGLL(int Na, int Np, bool verbose = false);
};

class AngularQuadratureProdGLC : public ProductQuadrature
{
public:
  /**Constructor for Angular Gauss-Legendre-Chebyshev.*/
  explicit AngularQuadratureProdGLC(int Na, int Np, bool verbose = false);
};

class AngularQuadratureProdCustom : public ProductQuadrature
{
public:
  /**Constructor for Custom Angular Product Quadrature.*/
  AngularQuadratureProdCustom(const std::vector<double>& azimuthal,
                              const std::vector<double>& polar,
                              const std::vector<double>& in_weights,
                              bool verbose);
};

} // namespace opensn
