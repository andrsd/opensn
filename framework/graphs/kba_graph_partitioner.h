#pragma once

#include "framework/graphs/graph_partitioner.h"

#include <array>

namespace chi
{

class KBAGraphPartitioner : public GraphPartitioner
{
public:
  static InputParameters GetInputParameters();
  explicit KBAGraphPartitioner(opensn::App& app);
  explicit KBAGraphPartitioner(const InputParameters& params);

  std::vector<int64_t> Partition(const std::vector<std::vector<uint64_t>>& graph,
                                 const std::vector<chi_mesh::Vector3>& centroids,
                                 int number_of_parts) override;

protected:
  size_t nx_, ny_, nz_;
  std::vector<double> xcuts_, ycuts_, zcuts_;

  struct CoordinateInfo
  {
    std::vector<double>* cuts_;
    size_t n_;
    std::string coordinate_name_;
  };
  std::array<CoordinateInfo, 3> coordinate_infos_;
};

} // namespace chi
