#pragma once

#include "framework/object.h"

namespace chi_mesh
{

/**Base class for mesh modifiers*/
class MeshModifier : public chi::ChiObject
{
public:
  explicit MeshModifier(const chi::InputParameters& params);
  virtual ~MeshModifier() = default;

  virtual void Apply() = 0;
};

} // namespace chi_mesh
