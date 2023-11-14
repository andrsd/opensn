#pragma once

#include "framework/object.h"

namespace chi
{

/**A generic material object used to group together multiple properties.*/
class Material : public ChiObject
{
private:
  std::string name_;

public:
  static InputParameters GetInputParameters();
  explicit Material(opensn::App& app, const chi::InputParameters& params);
};

} // namespace chi
