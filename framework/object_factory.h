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
 * \note Remember to include the header "Object/ChiObjectFactory.h".*/
#define RegisterChiObject(namespace_name, object_name)                                             \
  static char ChiObjectFactoryJoinWordsB(unique_var_name_object_##object_name##_, __COUNTER__) =   \
    opensn::ChiObjectFactory::AddObjectToRegistry<object_name, Object>(#namespace_name,            \
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
 * \note Remember to include the header "Object/object_maker.h"*/
#define RegisterChiObjectParametersOnly(namespace_name, object_name)                               \
  static char ChiObjectFactoryJoinWordsB(unique_var_name_object_##object_name##_, __COUNTER__) =   \
    opensn::ChiObjectFactory::AddObjectToRegistryParamsOnly<object_name>(#namespace_name,          \
                                                                         #object_name)

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
 * \note Remember to include the header "Object/object_maker.h"*/
#define RegisterSyntaxBlock(namespace_name, block_name, syntax_function)                           \
  static char ChiObjectFactoryJoinWordsB(unique_var_name_syntax_##block_name##_, __COUNTER__) =    \
    opensn::ChiObjectFactory::AddSyntaxBlockToRegistry(                                            \
      #namespace_name, #block_name, syntax_function)

namespace opensn
{

class Object;

/**Singleton object for handling the registration and making of ChiObjects.*/
class ChiObjectFactory
{
public:
  using ObjectPtr = std::shared_ptr<Object>;

  using ObjectGetInParamsFunc = InputParameters (*)();
  using ObjectConstructorFunc = ObjectPtr (*)(const InputParameters&);

  // Structure storing the entities necessary for creating an object
  struct ObjectRegistryEntry
  {
    ObjectGetInParamsFunc get_in_params_func = nullptr;
    ObjectConstructorFunc constructor_func = nullptr;
  };

  // Deleted copy, move constructors and copy assignment operator
  ChiObjectFactory(const ChiObjectFactory&) = delete;
  ChiObjectFactory(const ChiObjectFactory&&) = delete;
  ChiObjectFactory& operator=(const ChiObjectFactory&) = delete;

  /**Access to the singleton*/
  static ChiObjectFactory& GetInstance() noexcept;

  /**Returns a constant reference to the object registry.*/
  const std::map<std::string, ObjectRegistryEntry>& Registry() const;
  /**Checks if the object registry has a specific text key.*/
  bool RegistryHasKey(const std::string& key) const;

  template <typename T, typename base_T>
  static char AddObjectToRegistry(const std::string& namespace_name, const std::string& object_name)
  {
    auto& object_maker = GetInstance();

    const std::string name = namespace_name + "::" + object_name;
    object_maker.AssertRegistryKeyAvailable(name, __PRETTY_FUNCTION__);

    ObjectRegistryEntry reg_entry;
    reg_entry.get_in_params_func = &CallGetInputParamsFunction<T>;
    reg_entry.constructor_func = &CallObjectConstructor<T, base_T>;
    object_maker.object_registry_.insert(std::make_pair(name, reg_entry));

    return 0;
  }

  template <typename T>
  static char AddObjectToRegistryParamsOnly(const std::string& namespace_name,
                                            const std::string& object_name)
  {
    auto& object_maker = GetInstance();

    const std::string name = namespace_name + "::" + object_name;
    object_maker.AssertRegistryKeyAvailable(name, __PRETTY_FUNCTION__);

    ObjectRegistryEntry reg_entry;
    reg_entry.get_in_params_func = &CallGetInputParamsFunction<T>;
    object_maker.object_registry_.insert(std::make_pair(name, reg_entry));

    return 0;
  }

  static char AddSyntaxBlockToRegistry(const std::string& namespace_name,
                                       const std::string& block_name,
                                       ObjectGetInParamsFunc syntax_function)
  {
    auto& object_maker = GetInstance();

    const std::string name = namespace_name + "::" + block_name;
    object_maker.AssertRegistryKeyAvailable(name, __PRETTY_FUNCTION__);

    ObjectRegistryEntry reg_entry;
    reg_entry.get_in_params_func = syntax_function;
    object_maker.object_registry_.insert(std::make_pair(name, reg_entry));

    return 0;
  }

  /**Makes an object with the given parameters and places on the global
   * object stack. Returns a handle to the object. The object type is
   * obtained from a string parameter name `chi_obj_type`.*/
  size_t MakeRegisteredObject(const ParameterBlock& params) const;
  /**Makes an object with the given parameters and places on the global
   * object stack. Returns a handle to the object.*/
  size_t MakeRegisteredObjectOfType(const std::string& type, const ParameterBlock& params) const;

  /**Returns the input parameters of a registered object.*/
  InputParameters GetRegisteredObjectParameters(const std::string& type) const;

  /**\brief Dumps the object registry to stdout.*/
  void DumpRegister() const;

private:
  std::map<std::string, ObjectRegistryEntry> object_registry_;

  /**Private constructor because this is a singleton.*/
  ChiObjectFactory() = default;

  /**Utility redirection to call an object's static `GetInputParameters`
   * function.*/
  template <typename T>
  static InputParameters CallGetInputParamsFunction()
  {
    return T::GetInputParameters();
  }

  /**Utility redirection to call an object's constructor with a specified list
   * of input parameters.*/
  template <typename T, typename base_T>
  static std::shared_ptr<base_T> CallObjectConstructor(const InputParameters& params)
  {
    return std::make_shared<T>(params);
  }

  /**Checks that the registry key is available and throws a
   * `std::logical_error` if it is not.*/
  void AssertRegistryKeyAvailable(const std::string& key,
                                  const std::string& calling_function) const;
};

} // namespace opensn
