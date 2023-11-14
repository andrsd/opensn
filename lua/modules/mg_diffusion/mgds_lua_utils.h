#pragma once

#include "lua/base/lua.h"
#include "modules/mg_diffusion/mg_diffusion_solver.h"

namespace mg_diffusion::mgd_lua_utils
{
/** Creates a Multigroup CFEM Diffusion solver.
 *
 * \return Handle int Handle to the created solver.
 * \ingroup LuaDiffusion
 */
int chiCFEMMGDiffusionSolverCreate(lua_State* L);

/** Sets a property of a Diffusion solver. Please also consult the whitepaper
 * for the Diffusion solver (<a
 * href="../../whitepages/DiffusionSolver/DiffusionSolver.pdf">
 * Diffusion Whitepaper</a>)
 *
 * \n\n Additional basic options can be set as indicated in \ref
 * LuaDiffusionBasicOptions
 *
 * \param SolverHandle int Handle to an existing diffusion solver.
 * \param PropertyName string Name for a specific property.
 * \param Values varying Number of inputs associated with the index.<br>
 *
 * ##_
 *
 * ###PropertyName\n
 * "boundary_type"\n
 *  Boundary type. Expects boundary index then <B>BoundaryTypeName</B>
 *  then type value.\n\n
 *
 * \code
 * chiCFEMMGDiffusionSetBCProperty(solver,"boundary_type",bdID,"vacuum")
 * \endcode
 *
 * ### BoundaryTypeName
 * reflecting\n
 *  Reflecting boundary conditions. Synonymous with Neumann with a
 *  derivative of 0.0.
 *              \f[ -D \hat{n}\cdot \nabla \phi = 0 \f]\n\n
 *
 * neumann\n
 *  Constant derivative boundary condition. Expects to be followed
 *  by a constant \f$ f \f$ representing
 *                     \f[ -D \hat{n}\cdot \nabla \phi = f \f]\n\n
 *
 * vacuum\n
 *  Vacuum boundary conditions. More appropriate to neutron diffusion.
 *    \f[ \frac{1}{4}\phi + \frac{1}{2} D \hat{n}\cdot \nabla \phi = 0 \f]\n\n
 *
 * robin\n
 *  Robin boundary condition of the form
 *                    \f[ a \phi + b D \hat{n}\cdot \nabla \phi = f \f]\n\n
 *
 * \ingroup LuaDiffusion
 * \author Jean
 */
int chiCFEMMGDiffusionSetBCProperty(lua_State* L);

void RegisterLuaEntities(lua_State* L);
} // namespace mg_diffusion::mgd_lua_utils
