#include "lua-ng/framework.h"
#include "framework/mesh/volume_mesher/volume_mesher.h"
#include "framework/mesh/mesh_generator/mesh_generator.h"
#include "framework/mesh/mesh_generator/orthogonal_mesh_generator.h"
#include "framework/mesh/logical_volume/boolean_logical_volume.h"
#include "framework/mesh/logical_volume/rcc_logical_volume.h"
#include "framework/mesh/logical_volume/rpp_logical_volume.h"
#include "framework/mesh/logical_volume/sphere_logical_volume.h"
#include "framework/mesh/logical_volume/surface_mesh_logical_volume.h"

using namespace luaaa;
using namespace opensn;

namespace opensnluang
{

void
BindFramework(lua_State* state)
{
  LuaClass<BooleanLogicalVolume>(state, "BooleanLogicalVolume")
    .ctor<const InputParameters&>("Create")
    .fun("Inside", &BooleanLogicalVolume::Inside);

  LuaClass<RCCLogicalVolume>(state, "RCCLogicalVolume")
    .ctor<const InputParameters&>("Create")
    .fun("Inside", &RCCLogicalVolume::Inside);

  //  LuaClass<RPPLogicalVolume> l_rpp(state, "RPPLogicalVolume");
  //  l_rpp.ctor<const InputParameters&>("Create");
  //  l_rpp.fun("Inside", &RPPLogicalVolume::Inside);

  LuaClass<SphereLogicalVolume>(state, "SphereLogicalVolume")
    .ctor<const InputParameters&>("Create")
    .fun("Inside", &SphereLogicalVolume::Inside);

  LuaClass<SurfaceMeshLogicalVolume>(state, "SurfaceMeshLogicalVolume")
    .ctor<const InputParameters&>("Create")
    .fun("Inside", &SurfaceMeshLogicalVolume::Inside);

  //  LuaClass<UnpartitionedMesh>(state, "UnpartitionedMesh")
  //    .ctor<const InputParameters&>("Create");

  LuaClass<MeshGenerator>(state, "MeshGenerator")
    .ctor2("Create")
//    .fun("GenerateUnpartitionedMesh", &MeshGenerator::GenerateUnpartitionedMesh);
    .fun("Execute", &MeshGenerator::Execute);

  LuaClass<OrthogonalMeshGenerator>(state, "OrthogonalMeshGenerator")
    .ctor2("Create")
    .fun("Execute", &OrthogonalMeshGenerator::Execute);

  LuaModule(state, "VolumeMesher")
    //        .fun("GetCellXYPartitionID", &VolumeMesher::GetCellXYPartitionID)
    //        .fun("GetCellXYZPartitionID", &VolumeMesher::GetCellXYZPartitionID)
    .fun("CreatePolygonCells", &VolumeMesher::CreatePolygonCells)
    .fun("SetMatIDFromLogical", &VolumeMesher::SetMatIDFromLogical)
    .fun("SetBndryIDFromLogical", &VolumeMesher::SetBndryIDFromLogical)
    .fun("SetMatIDToAll", &VolumeMesher::SetMatIDToAll)
    .fun("SetupOrthogonalBoundaries", &VolumeMesher::SetupOrthogonalBoundaries);
}

} // namespace opensnluang
