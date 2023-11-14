#pragma once

#include "framework/object.h"

namespace opensn
{

/**
 * Maintains mapping from object type to an entry that describes how to build such an object
 */
class Registry
{
private:
  using ObjectPtr = std::shared_ptr<chi::ChiObject>;
  using ObjectGetInParamsFunc = chi::InputParameters (*)();
  using ObjectConstructorFunc = ObjectPtr (*)(App& app, const chi::InputParameters&);

  // Structure storing the entities necessary for creating an object
  struct ObjectRegistryEntry
  {
    ObjectGetInParamsFunc get_in_params_func = nullptr;
    ObjectConstructorFunc constructor_func = nullptr;
  };

public:
  /**
   * Get the instance of registry
   */
  static Registry& Instance();

  /**
   * Checks if the object registry has a specific text key.
   */
  bool HasKey(const std::string& key);

  /**
   * Register object
   *
   * \tparam T Object type
   * \param namespace_name Namespace
   * \param object_name Object name
   */
  template <typename T, typename base_T>
  char AddObject(const std::string& namespace_name, const std::string& object_name)
  {
    const std::string name = namespace_name + "::" + object_name;
    AssertRegistryKeyAvailable(name, __PRETTY_FUNCTION__);

    ObjectRegistryEntry reg_entry;
    reg_entry.get_in_params_func = &CallGetInputParamsFunction<T>;
    reg_entry.constructor_func = &CallObjectConstructor<T, base_T>;
    object_registry_.insert(std::make_pair(name, reg_entry));

    return '\0';
  }

  /**
   * Register object for build parameters only
   *
   * \tparam T Object type
   * \param namespace_name Namespace
   * \param object_name Object name
   */
  template <typename T>
  char AddObjectParamsOnly(const std::string& namespace_name, const std::string& object_name)
  {
    const std::string name = namespace_name + "::" + object_name;
    AssertRegistryKeyAvailable(name, __PRETTY_FUNCTION__);

    ObjectRegistryEntry reg_entry;
    reg_entry.get_in_params_func = &CallGetInputParamsFunction<T>;
    object_registry_.insert(std::make_pair(name, reg_entry));

    return '\0';
  }

  /**
   * Register syntax block
   *
   * \param namespace_name Namespace only
   * \param block_name Block name
   * \param syntax_function Syntax function
   */
  char AddSyntaxBlock(const std::string& namespace_name,
                      const std::string& block_name,
                      ObjectGetInParamsFunc syntax_function)
  {
    const std::string name = namespace_name + "::" + block_name;
    AssertRegistryKeyAvailable(name, __PRETTY_FUNCTION__);

    ObjectRegistryEntry reg_entry;
    reg_entry.get_in_params_func = syntax_function;
    object_registry_.insert(std::make_pair(name, reg_entry));

    return '\0';
  }

  /**
   * Find a registry entry given the object type
   *
   * \param type Object type
   * \return Registry entry for the `type`
   */
  const ObjectRegistryEntry& Find(const std::string& type) const;

  /**
   * Dumps the object registry to stdout.
   *
   * \param stream Output stream that will receive the content
   * \param verbosity Verbosity level
   */
  void Dump(std::ostream& stream, int verbosity) const;

private:
  Registry() = default;

  /**
   * Utility redirection to call an object's static `GetInputParameters` function.
   */
  template <typename T>
  static chi::InputParameters CallGetInputParamsFunction()
  {
    return T::GetInputParameters();
  }

  /**
   * Utility redirection to call an object's constructor with a specified list of input parameters.
   */
  template <typename T, typename base_T>
  static std::shared_ptr<base_T> CallObjectConstructor(App& app, const chi::InputParameters& params)
  {
    return std::make_shared<T>(app, params);
  }

  /**
   * Checks that the registry key is available and throws a `std::logical_error` if it is not.
   */
  void AssertRegistryKeyAvailable(const std::string& key, const std::string& calling_function);

  /// Registry entries per object name
  std::map<std::string, ObjectRegistryEntry> object_registry_;
};

} // namespace opensn
