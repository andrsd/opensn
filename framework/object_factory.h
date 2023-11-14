#pragma once

#include "framework/parameters/input_parameters.h"
#include "framework/object.h"
#include "framework/logging/log_exceptions.h"

/**Small utility macro for joining two words.*/
#define ChiObjectFactoryJoinWordsA(x, y) x##y
/**IDK why this is needed. Seems like counter doesnt work properly without it*/
#define ChiObjectFactoryJoinWordsB(x, y) ChiObjectFactoryJoinWordsA(x, y)

/**Macro for registering an object within the ChiObjectFactory singleton.
 * \param namespace_name Name of the namespace within which the object is.
 * \param object_name Name of the object in the registry.
 * Example:
 * \code
 * RegisterChiObject(kaka, Zorba);
 * \endcode
 * \note Remember to include the header "ChiObject/ChiObjectFactory.h".*/
#define RegisterChiObject(namespace_name, object_name)                                             \
  static char ChiObjectFactoryJoinWordsB(unique_var_name_object_##object_name##_, __COUNTER__) =   \
    ChiObjectFactory::AddObjectToRegistry<object_name, chi::ChiObject>(#namespace_name,            \
                                                                       #object_name)

/**Macro for registering an object (parameters only) within the
 * ChiObjectFactory singleton.
 * \param namespace_name Name of the namespace within which the object is.
 * \param object_name Name of the object in the registry.
 * Example:
 * \code
 * RegisterChiObjectParametersOnly(kaka, Zorba);
 * \endcode
 *
 * \note Remember to include the header "ChiObject/object_maker.h"*/
#define RegisterChiObjectParametersOnly(namespace_name, object_name)                               \
  static char ChiObjectFactoryJoinWordsB(unique_var_name_object_##object_name##_, __COUNTER__) =   \
    ChiObjectFactory::AddObjectToRegistryParamsOnly<object_name>(#namespace_name, #object_name)

/**Macro for registering a pure input parameters block within the
 * ChiObjectFactory singleton AND giving it a custom name
 * \param namespace_name Name of the namespace within which the object is.
 * \param block_name Name of the object in the registry.
 * \param syntax_function Actual syntax function for this object
 * Example:
 * \code
 * RegisterSyntaxBlock(kaka, Zorba, ZorbaSyntaxFunction);
 * \endcode
 *
 * \note Remember to include the header "ChiObject/object_maker.h"*/
#define RegisterSyntaxBlock(namespace_name, block_name, syntax_function)                           \
  static char ChiObjectFactoryJoinWordsB(unique_var_name_syntax_##block_name##_, __COUNTER__) =    \
    ChiObjectFactory::AddSyntaxBlockToRegistry(#namespace_name, #block_name, syntax_function)

/**Singleton object for handling the registration and making of ChiObjects.*/
class ChiObjectFactory
{
public:
  using ObjectPtr = std::shared_ptr<chi::ChiObject>;

  using ObjectGetInParamsFunc = chi::InputParameters (*)();
  using ObjectConstructorFunc = ObjectPtr (*)(opensn::App& app, const chi::InputParameters&);

  // Structure storing the entities necessary for creating an object
  struct ObjectRegistryEntry
  {
    ObjectGetInParamsFunc get_in_params_func = nullptr;
    ObjectConstructorFunc constructor_func = nullptr;
  };

  ChiObjectFactory(opensn::App& app);
  // Deleted copy, move constructors and copy assignment operator
  ChiObjectFactory(const ChiObjectFactory&) = delete;
  ChiObjectFactory(const ChiObjectFactory&&) = delete;
  ChiObjectFactory& operator=(const ChiObjectFactory&) = delete;

  /**Returns a constant reference to the object registry.*/
  static const std::map<std::string, ObjectRegistryEntry>& Registry();
  /**Checks if the object registry has a specific text key.*/
  static bool RegistryHasKey(const std::string& key);

  template <typename T, typename base_T>
  static char AddObjectToRegistry(const std::string& namespace_name, const std::string& object_name)
  {
    const std::string name = namespace_name + "::" + object_name;
    AssertRegistryKeyAvailable(name, __PRETTY_FUNCTION__);

    ObjectRegistryEntry reg_entry;
    reg_entry.get_in_params_func = &CallGetInputParamsFunction<T>;
    reg_entry.constructor_func = &CallObjectConstructor<T, base_T>;
    object_registry_.insert(std::make_pair(name, reg_entry));

    return 0;
  }

  template <typename T>
  static char AddObjectToRegistryParamsOnly(const std::string& namespace_name,
                                            const std::string& object_name)
  {
    const std::string name = namespace_name + "::" + object_name;
    AssertRegistryKeyAvailable(name, __PRETTY_FUNCTION__);

    ObjectRegistryEntry reg_entry;
    reg_entry.get_in_params_func = &CallGetInputParamsFunction<T>;
    object_registry_.insert(std::make_pair(name, reg_entry));

    return 0;
  }

  static char AddSyntaxBlockToRegistry(const std::string& namespace_name,
                                       const std::string& block_name,
                                       ObjectGetInParamsFunc syntax_function)
  {
    const std::string name = namespace_name + "::" + block_name;
    AssertRegistryKeyAvailable(name, __PRETTY_FUNCTION__);

    ObjectRegistryEntry reg_entry;
    reg_entry.get_in_params_func = syntax_function;
    object_registry_.insert(std::make_pair(name, reg_entry));

    return 0;
  }

  /**Makes an object with the given parameters and places on the global
   * object stack. Returns a handle to the object. The object type is
   * obtained from a string parameter name `chi_obj_type`.*/
  size_t MakeRegisteredObject(const chi::ParameterBlock& params) const;
  /**Makes an object with the given parameters and places on the global
   * object stack. Returns a handle to the object.*/
  size_t MakeRegisteredObjectOfType(const std::string& type,
                                    const chi::ParameterBlock& params) const;

  /**Returns the input parameters of a registered object.*/
  static chi::InputParameters GetRegisteredObjectParameters(const std::string& type);

  /**\brief Dumps the object registry to stdout.*/
  void DumpRegister() const;

private:
  opensn::App& app_;
  static std::map<std::string, ObjectRegistryEntry> object_registry_;

  /**Utility redirection to call an object's static `GetInputParameters`
   * function.*/
  template <typename T>
  static chi::InputParameters CallGetInputParamsFunction()
  {
    return T::GetInputParameters();
  }

  /**Utility redirection to call an object's constructor with a specified list
   * of input parameters.*/
  template <typename T, typename base_T>
  static std::shared_ptr<base_T> CallObjectConstructor(opensn::App& app,
                                                       const chi::InputParameters& params)
  {
    return std::make_shared<T>(app, params);
  }

  /**Checks that the registry key is available and throws a
   * `std::logical_error` if it is not.*/
  static void AssertRegistryKeyAvailable(const std::string& key,
                                         const std::string& calling_function);
};
