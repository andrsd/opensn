#include "lua/framework/runtime.h"
#include "framework/event_system/system_wide_event_publisher.h"
#include "framework/utils/timer.h"

std::vector<chi_mesh::SurfaceMeshPtr> Chi::surface_mesh_stack;
std::vector<chi_mesh::FFInterpPtr> Chi::field_func_interpolation_stack;
std::vector<chi_mesh::UnpartMeshPtr> Chi::unpartitionedmesh_stack;
