#pragma once

#include "framework/chi_lua.h"

int chiSurfaceMeshCreate(lua_State* L);
int chiSurfaceMeshImportFromOBJFile(lua_State* L);
int chiSurfaceMeshImportFromTriangleFiles(lua_State* L);

int chiSurfaceMeshExportToObj(lua_State* L);
int chiSurfaceMeshExportPolyFile(lua_State* L);

int chiSurfaceMeshCheckCycles(lua_State* L);
int chiComputeLoadBalancing(lua_State* L);
