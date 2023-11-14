#pragma once

#include "framework/parameters/input_parameters.h"
#include "framework/object.h"
#include "framework/logging/log_exceptions.h"
#include "framework/registry.h"

/**
 * Macro for joining two words.
 */
#define ChiObjectFactoryJoinWordsA(x, y) x##y

/**
 * IDK why this is needed. Seems like counter doesn't work properly without it
 */
#define ChiObjectFactoryJoinWordsB(x, y) ChiObjectFactoryJoinWordsA(x, y)

/**
 * Macro for registering an object with the Registry
 *
 * \param namespace_name Name of the namespace within which the object is.
 * \param object_name Name of the object in the registry.
 * Example:
 * \code
 * RegisterChiObject(kaka, Zorba);
 * \endcode
 * \note Remember to include the header "framework/object_factory.h".
 */
#define RegisterChiObject(namespace_name, object_name)                                             \
  static char ChiObjectFactoryJoinWordsB(unique_var_name_object_##object_name##_, __COUNTER__) =   \
    opensn::Registry::Instance().AddObject<object_name, chi::ChiObject>(#namespace_name,           \
                                                                        #object_name)

/**
 * Macro for registering an object (parameters only) the Registry
 *
 * \param namespace_name Name of the namespace within which the object is.
 * \param object_name Name of the object in the registry.
 * Example:
 * \code
 * RegisterChiObjectParametersOnly(kaka, Zorba);
 * \endcode
 *
 * \note Remember to include the header "framework/object_factory.h"
 */
#define RegisterChiObjectParametersOnly(namespace_name, object_name)                               \
  static char ChiObjectFactoryJoinWordsB(unique_var_name_object_##object_name##_, __COUNTER__) =   \
    opensn::Registry::Instance().AddObjectParamsOnly<object_name>(#namespace_name, #object_name)

/**
 * Macro for registering a pure input parameters block in the Registry AND giving it a custom name
 *
 * \param namespace_name Name of the namespace within which the object is.
 * \param block_name Name of the object in the registry.
 * \param syntax_function Actual syntax function for this object
 * Example:
 * \code
 * RegisterSyntaxBlock(kaka, Zorba, ZorbaSyntaxFunction);
 * \endcode
 *
 * \note Remember to include the header "framework/object_factory.h"
 */
#define RegisterSyntaxBlock(namespace_name, block_name, syntax_function)                           \
  static char ChiObjectFactoryJoinWordsB(unique_var_name_syntax_##block_name##_, __COUNTER__) =    \
    opensn::Registry::Instance().AddSyntaxBlock(#namespace_name, #block_name, syntax_function)

namespace opensn
{

/**
 * Factory for creating registered objects
 */
class ObjectFactory
{
public:
  explicit ObjectFactory(opensn::App& app);
  // Deleted copy, move constructors and copy assignment operator
  ObjectFactory(const ObjectFactory&) = delete;
  ObjectFactory(const ObjectFactory&&) = delete;
  ObjectFactory& operator=(const ObjectFactory&) = delete;

  /**
   * Makes an object with the given parameters and places on the global object stack. Returns a
   * handle to the object. The object type is obtained from a string parameter name `chi_obj_type`.
   *
   * \param params Parameter block
   */
  size_t MakeRegisteredObject(const chi::ParameterBlock& params) const;

  /**
   * Makes an object with the given parameters and places on the global object stack. Returns a
   * handle to the object.
   *
   * \param type Object type
   * \param params Parameter block
   */
  size_t MakeRegisteredObjectOfType(const std::string& type,
                                    const chi::ParameterBlock& params) const;

  /**
   * Returns the input parameters of a registered object.
   *
   * \param type Object type
   */
  chi::InputParameters GetRegisteredObjectParameters(const std::string& type);

private:
  /// Reference to the appliction using this factory
  opensn::App& app_;
};

} // namespace opensn
