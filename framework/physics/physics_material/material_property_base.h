#pragma once

#include <string>
#include <vector>

namespace opensn
{
class App;
}

namespace chi_physics
{
enum class PropertyType
{
  SCALAR_VALUE = 1,
  TRANSPORT_XSECTIONS = 10,
  ISOTROPIC_MG_SOURCE = 11
};

/** Base class for material properties.*/
class MaterialProperty
{
private:
  opensn::App& app_;
  const PropertyType type_;

public:
  std::string property_name;

  explicit MaterialProperty(opensn::App& app, PropertyType in_type) : app_(app), type_(in_type) {}

  virtual ~MaterialProperty() = default;

  opensn::App& App() const { return app_; }

  PropertyType Type() { return type_; }

  virtual double GetScalarValue() { return 0.0; }
};

} // namespace chi_physics
