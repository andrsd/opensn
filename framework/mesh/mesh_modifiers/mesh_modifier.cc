#include "framework/mesh/mesh_modifiers/mesh_modifier.h"

namespace chi_mesh
{

MeshModifier::MeshModifier(opensn::App& app, const chi::InputParameters& params)
  : ChiObject(app, params)
{
}

} // namespace chi_mesh
