#pragma once

#include "framework/mesh/mesh_vector.h"

#include <map>

namespace opensn
{

/**Manages a vertex map with custom calls.*/
class VertexHandler
{
  typedef std::map<uint64_t, Vector3> GlobalIDMap;

private:
  std::map<uint64_t, Vector3> m_global_id_vertex_map;

public:
  // Iterators
  GlobalIDMap::iterator begin() { return m_global_id_vertex_map.begin(); }
  GlobalIDMap::iterator end() { return m_global_id_vertex_map.end(); }

  GlobalIDMap::const_iterator begin() const { return m_global_id_vertex_map.begin(); }
  GlobalIDMap::const_iterator end() const { return m_global_id_vertex_map.end(); }

  // Accessors
  Vector3& operator[](const uint64_t global_id) { return m_global_id_vertex_map.at(global_id); }

  const Vector3& operator[](const uint64_t global_id) const
  {
    return m_global_id_vertex_map.at(global_id);
  }

  // Utilities
  void Insert(const uint64_t global_id, const Vector3& vec)
  {
    m_global_id_vertex_map.insert(std::make_pair(global_id, vec));
  }

  size_t NumLocallyStored() const { return m_global_id_vertex_map.size(); }

  void Clear() { m_global_id_vertex_map.clear(); }
};

} // namespace opensn
