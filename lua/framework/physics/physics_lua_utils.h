#pragma once

#include "framework/lua.h"

/** Adds a material to the problem. Materials are added to the global
 * physics handler and is therefore accessible across all meshes and solvers.
 *
 * \param Name char (Optional) Material name.
 *
 * \return MaterialHandle int Handle to the created material.
 *
 *
 * ##_
 *
 * ### Example\n
 * Example lua code:
 * \code
 * materials = {}
 * materials[1] = PhysicsAddMaterial("Test Material");
 * \endcode
 *
 *
 * \ingroup LuaPhysicsMaterials
 * \author Jan
 */
int PhysicsAddMaterial(lua_State* L);

/** Adds a material property to a material.
 *
 * \param MaterialHandle int Index to the reference material.
 * \param PropertyIndex int Property index.
 *
 * ##_
 *
 * ###PropertyIndex\n
 *
 * SCALAR_VALUE\n
 *  Simple scalar value property.\n\n
 *
 * TRANSPORT_XSECTIONS\n
 *  Multi-group transport cross-section supporting numerous features.\n\n
 *
 * ISOTROPIC_MG_SOURCE\n
 *  Isotropic Multigroup Source.\n\n
 *
 * ### Developer Info
 * Checklist for adding a new material property:
 *  - Create your property class in its own header file. i.e.
 *    "physics/PhysicsMaterial/property_xx_myproperty.h"
 *  - Add the property to the physics namespace
 *    ("physics/chi_physics_namespace.h"). Make sure to derive from the base
 *    class.
 *  - Go define the integer to be associated with your new property in
 *    chi_physicsmaterial.h
 *  - Include the header file for your property in this file (i.e. at the top).
 *  - Add this property integer in the lua register (ChiLua/chi_lua_register.h).
 *    For testing you can just use the integer value but eventually you'd want
 *    to supply an easier way for users to enter it.
 *  - Add another else-if for your property. Just have a look at how the others
 *    were done, it should be intuitive enough.
 *
 * ##_
 *
 * ### Example\n
 * Example lua code:
 * \code
 * PhysicsMaterialAddProperty(materials[i],TRANSPORT_XSECTIONS)
 * \endcode
 *
 * \ingroup LuaPhysicsMaterials
 * \author Jan
 */
int PhysicsMaterialAddProperty(lua_State* L);

/** Sets a material property for a given material.
 *
 * \param MaterialHandle int Index to the reference material.
 * \param PropertyIndex int Property index. Or name of property.
 * \param OperationIndex int Method used for setting the material property.
 * \param Information varying Varying information depending on the operation.
 *
 * ##_
 *
 * ###PropertyIndex\n
 * SCALAR_VALUE         =  Basic Scalar value property.\n
 * TRANSPORT_XSECTIONS   =  Multi-group transport cross-section supporting numerous
 *                         features.\n
 * ISOTROPIC_MG_SOURCE = Isotropic Multigroup Source.\n
 *
 * ###OperationIndex\n
 * SINGLE_VALUE\n
 * Sets the property based on a single value. Requires a single value as additional
 * information. As a simple example consider the case where the user would like
 * to set a single constant thermal conductivity. This can be achieved with \n
 * FROM_ARRAY\n
 * Sets a property based on a Lua array indexed from 1 to N. Internally
 * will be converted to 0 to N-1. This method can be used to set mutligroup
 * cross-sections or sources.
 * \n
 * SIMPLEXS0\n
 * Makes a simple material with no transfer matrix just \f$\sigma_t \f$. Expects
 * two values: \n
 *  - int number of groups \f$G \f$,
 *  - float \f$\sigma_t \f$.
 *
 * ####_
 *
 * SIMPLEXS1\n
 * Makes a simple material with isotropic transfer matrix (L=0)
 * and mostly down scattering but with a few of the last groups
 * subject to up-scattering. Expects three values
 * values: \n
 *  - int number of groups (\f$G \f$),
 *  - float \f$\sigma_t \f$,
 *  - float scattering to total ratio (\f$c \f$)
 *
 * ####_
 *
 * CHI_XSFILE\n
 * Loads transport cross-sections from CHI type cross-section files. Expects
 * to be followed by a filepath specifying the xs-file.
 *
 * ####_
 *
 * EXISTING\n
 * Supply handle to an existing cross-section and simply swap them out.
 *
 * \code
 * chiPhysicsMaterialSetProperty(materials[1],
 *                               TRANSPORT_XSECTIONS,
 *                               CHI_XSFILE,
 *                               "xs_3_170.cxs",
 *                               "2518")
 * \endcode
 *
 * ##_
 *
 * ### Example 1
 * Simple temperature independent thermal conductivity:
 * \code
 * materials = {}
 * materials[1] = PhysicsAddMaterial("Test Material");
 * PhysicsMaterialAddProperty(materials[0],THERMAL_CONDUCTIVITY)
 * chiPhysicsMaterialSetProperty(materials[0],THERMAL_CONDUCTIVITY,SINGLE_VALUE,13.7)
 * \endcode
 *
 * where the thermal conductivity has been set to 13.7.\n
 *
 * ### Example 2
 * Isotropic Multigroup source set from a lua table/array (12 groups):
 * \code
 * materials = {}
 * materials[1] = PhysicsAddMaterial("Test Material");
 *
 * PhysicsMaterialAddProperty(materials[1],ISOTROPIC_MG_SOURCE)
 *
 * num_groups = 12
 * src={}
 * for g=1,num_groups do
 *     src[g] = 0.0
 * end
 * chiPhysicsMaterialSetProperty(materials[1],ISOTROPIC_MG_SOURCE,FROM_ARRAY,src)
 * \endcode
 *
 * ### Developer Info
 * Checklist for adding a new material property:
 *  - Make sure you followed the steps depicted in the developer info section for
 *    the ChiLua::PhysicsMaterialAddProperty function.
 *  - Now under the "If user supplied name then find property index"-section
 *    add the appropriate code for setting the property index.
 *  - Add an else-if block for your property similar to the others. It should be
 *    intuitive if you look at the others.
 *  - Remember to add the filtering section if you need to support multiple type
 *    properties.
 *
 * \ingroup LuaPhysicsMaterials
 * \author Jan
 */
int chiPhysicsMaterialSetProperty(lua_State* L);

/** Returns a rich lua data-structure of the required property.
 *
 * \param MaterialHandle int Index to the reference material.
 * \param PropertyIndex int Property index. Or name of property.
 *
 *
 * \ingroup LuaPhysicsMaterials
 * \return Lua table of the desired property.
 */
int chiPhysicsMaterialGetProperty(lua_State* L);
