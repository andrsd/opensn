#include "framework/graphs/graph_partitioner.h"

namespace chi
{

InputParameters
GraphPartitioner::GetInputParameters()
{
  InputParameters params = ChiObject::GetInputParameters();

  return params;
}

GraphPartitioner::GraphPartitioner(opensn::App& app) : ChiObject(app)
{
}

GraphPartitioner::GraphPartitioner(opensn::App& app, const InputParameters& params)
  : ChiObject(app, params)
{
}

} // namespace chi
