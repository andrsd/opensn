#pragma once

#include "framework/post_processors/post_processor.h"
#include "framework/physics/field_function/grid_based_field_function_interface.h"
#include "framework/mesh/logical_volume/logical_volume_interface.h"

namespace chi_mesh
{
class LogicalVolume;
}
namespace chi_physics
{
class FieldFunctionGridBased;
}

namespace chi
{

class AggregateNodalValuePostProcessor : public PostProcessor,
                                         public chi_physics::GridBasedFieldFunctionInterface,
                                         public chi_mesh::LogicalVolumeInterface
{
public:
  static InputParameters GetInputParameters();
  explicit AggregateNodalValuePostProcessor(opensn::App& app, const InputParameters& params);

  void Execute(const opensn::Event& event_context) override;

protected:
  void Initialize();

  const std::string operation_;
  bool initialized_ = false;
  std::vector<uint64_t> cell_local_ids_;
};

} // namespace chi
