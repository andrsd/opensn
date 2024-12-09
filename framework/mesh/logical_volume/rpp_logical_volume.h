// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#pragma once

#include "framework/mesh/logical_volume/logical_volume.h"

namespace opensn
{

/// Rectangular Parallel Piped (RPP) logical volume
class RPPLogicalVolume : public LogicalVolume
{
  enum CornerName
  {
    XMIN = 0,
    XMAX = 1,
    YMIN = 2,
    YMAX = 3,
    ZMIN = 4,
    ZMAX = 5
  };

public:
  explicit RPPLogicalVolume(const InputParameters& params);

  bool Inside(const Vector3& point) const override;

protected:
  double xmin_, xmax_;
  double ymin_, ymax_;
  double zmin_, zmax_;
  bool infx_, infy_, infz_;

public:
  static InputParameters GetInputParameters();
  static std::shared_ptr<RPPLogicalVolume> Create(const ParameterBlock& params);
};

} // namespace opensn
