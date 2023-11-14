#pragma once

#include <utility>
#include <vector>
#include <string>
#include <stdexcept>
#include <memory>

#include "framework/mpi/mpi.h"

namespace chi_mesh
{
class MeshHandler;

class SurfaceMesh;
typedef std::shared_ptr<SurfaceMesh> SurfaceMeshPtr;

class FieldFunctionInterpolation;
typedef FieldFunctionInterpolation FFInterp;
typedef std::shared_ptr<FFInterp> FFInterpPtr;

class UnpartitionedMesh;
typedef std::shared_ptr<UnpartitionedMesh> UnpartitionedMeshPtr;
typedef UnpartitionedMeshPtr UnpartMeshPtr;
} // namespace chi_mesh

namespace chi_physics
{
class Solver;
class Material;
class MultiGroupXS;
class FieldFunction;

} // namespace chi_physics

namespace chi_math
{
class AngularQuadrature;
class SpatialDiscretization;

class UnknownManager;
} // namespace chi_math

namespace chi
{

class Timer;
class Console;
class ChiLog;
class PostProcessor;
/**Stores all the keys currently in the registries.*/
struct RegistryStatuses
{
  std::vector<std::string> objfactory_keys_;
  std::vector<std::string> console_lua_func_keys_;
  std::vector<std::string> console_lua_wrapper_keys_;
};

/**Simple structure for memory usage.*/
struct CSTMemory
{
  double memory_bytes = 0.0;
  double memory_kbytes = 0.0;
  double memory_mbytes = 0.0;
  double memory_gbytes = 0.0;

  CSTMemory() = default;

  explicit CSTMemory(double in_mem)
  {
    memory_bytes = in_mem;
    memory_kbytes = in_mem / 1024.0;
    memory_mbytes = in_mem / 1024.0 / 1024.0;
    memory_gbytes = in_mem / 1024.0 / 1024.0;
  }

  CSTMemory& operator=(const CSTMemory& in_struct) = default;
};

} // namespace chi

class ChiObject;
typedef std::shared_ptr<ChiObject> ChiObjectPtr;

/**General utilities in ChiTech*/
class Chi
{
public:
  static chi::MPI_Info& mpi;
  static chi::Timer program_timer;
  static chi::Console& console;
  static chi::ChiLog& log;

  /** Global stack of handlers */
  static std::vector<chi_mesh::MeshHandlerPtr> meshhandler_stack;
  static int current_mesh_handler;

  static std::vector<chi_mesh::SurfaceMeshPtr> surface_mesh_stack;
  static std::vector<chi_mesh::FFInterpPtr> field_func_interpolation_stack;
  static std::vector<chi_mesh::UnpartMeshPtr> unpartitionedmesh_stack;

  static std::vector<chi_physics::MaterialPtr> material_stack;
  static std::vector<chi_physics::MultiGroupXSPtr> multigroup_xs_stack;
  static std::vector<chi_physics::FieldFunctionPtr> field_function_stack;

  static std::vector<chi_math::AngularQuadraturePtr> angular_quadrature_stack;

  static std::vector<ChiObjectPtr> object_stack;
  static std::vector<chi_math::SpatialDiscretizationPtr> sdm_stack;
  static std::vector<chi::PostProcessorPtr> postprocessor_stack;

  /**
   * Get current memory usage.
   */
  static chi::CSTMemory GetMemoryUsage();

  /**
   * Get current memory usage in Megabytes.
   */
  static double GetMemoryUsageInMB();

  /**Data block for run-time quantities.*/
  class run_time
  {
  public:
    static bool termination_posted_;
    static std::string input_file_name_;
    static bool sim_option_interactive_;
    static bool allow_petsc_error_handler_;
    static bool supress_beg_end_timelog_;
    static bool suppress_color_;
    static bool dump_registry_;

    static const std::string command_line_help_string_;

    /**Parses input arguments.
    \param argc int    Number of arguments supplied.
    \param argv char** Array of strings representing each argument.
     */
    static void ParseArguments(int argc, char** argv);
    /**Initializes PetSc for use by all entities.*/
    static int InitPetSc(int argc, char** argv);

  public:
  public:
    /// Deleted constructor
    run_time() = delete;
    /// Deleted copy constructor
    run_time(const run_time&) = delete;
    /// Deleted assigment operator
    run_time operator=(const run_time&) = delete;
  };

  /**Customized exceptions.*/
  class RecoverableException : public std::runtime_error
  {
  public:
    explicit RecoverableException(const char* message)
      : std::runtime_error(std::string("RecoverableException: ") + std::string(message))
    {
    }
    explicit RecoverableException(const std::string& message)
      : std::runtime_error(std::string("RecoverableException: ") + message)
    {
    }
    RecoverableException(const std::string& prefix, const std::string& message)
      : std::runtime_error(prefix + message)
    {
    }

    ~RecoverableException() noexcept override = default;
  };

public:
  /// Deleted constructor
  Chi() = delete;
  /// Deleted copy constructor
  Chi(const Chi&) = delete;
  /// Deleted assigment operator
  Chi operator=(const Chi&) = delete;

public:
  /**Runs the interactive chitech engine*/
  static int RunInteractive(int argc, char** argv);
  /**Runs ChiTech in pure batch mode. Start then finish.*/
  static int RunBatch(int argc, char** argv);
  /**Initializes all necessary items for ChiTech.
  \param argc int    Number of arguments supplied.
  \param argv char** Array of strings representing each argument.
  \param communicator MPI_Comm The main communicator, used system wide.
   */
  static int Initialize(int argc, char** argv, MPI_Comm communicator);
  /**
   * Finalizes ChiTech.
   */
  static void Finalize();

  /**Builds a `RegistryStatuses` structure*/
  static chi::RegistryStatuses GetStatusOfRegistries();

  /** Gets the ChiTech-version string.*/
  static std::string GetVersionStr();

public:
  /**Attempts to retrieve an object of base-type `shared_ptr<T>` at the given
   * handle. It then attempts to cast it to type `shared_ptr<R>` and, if
   * successful, will return a reference of type R&.
   * \n
   * \n
   * Example usage:
   *
   * \code
   * const auto& surf_mesh = Chi::GetStackItem<chi_mesh::SurfaceMesh>(
        Chi::object_stack, surface_hndl);
     // Returns chi_mesh::SurfaceMesh&
   * \endcode
   * */
  ` template <class R, class T>
  static R& GetStackItem(std::vector<std::shared_ptr<T>>& stack,
                         const size_t handle,
                         const std::string& calling_function_name = "Unknown")
  {
    try
    {
      std::shared_ptr<T>& item = stack.at(handle);
      std::shared_ptr<R> ret_item = std::dynamic_pointer_cast<R>(item);
      if (not ret_item)
        throw std::logic_error("chi::GetStackItem: Invalid return type used. "
                               "Calling function: " +
                               calling_function_name);
      return *ret_item;
    }
    catch (const std::out_of_range& oor)
    {
      throw std::out_of_range("chi::GetStackItem: Invalid handle used. "
                              "Calling function: " +
                              calling_function_name);
    }
  }
  `
    /**Attempts to obtain object of type `shared_ptr<T>` at the given
     * handle of a stack with parent type P.
     * \n
     * \n
     * Example usage:
     *
     * \code
     * auto surf_mesh_ptr =
     *   Chi::GetStackItemPtrAsType<chi_mesh::SurfaceMesh>(
           Chi::object_stack, surf_mesh_hndle, fname);
       // Returns std::shared_ptr<chi_mesh::SurfaceMesh>
     * \endcode
     * */
    template <class T, class P>
    static std::shared_ptr<T>
    GetStackItemPtrAsType(std::vector<std::shared_ptr<P>>& stack,
                          const size_t handle,
                          const std::string& calling_function_name = "Unknown")
  {
    std::shared_ptr<P> item_type_P;
    try
    {
      item_type_P = stack.at(handle);
    }
    catch (const std::out_of_range& oor)
    {
      throw std::out_of_range("chi::GetStackItem: Invalid handle used. "
                              "Calling function: " +
                              calling_function_name);
    }

    auto item_type_T = std::dynamic_pointer_cast<T>(item_type_P);
    if (not item_type_T)
      throw std::logic_error(calling_function_name + "Failed to cast to requested type");

    return item_type_T;
  }

  /**Attempts to obtain object of type `shared_ptr<T>` at the given
   * handle of a stack ALSO OF TYPE T.
   * \n
   * \n
   * Example usage:
   *
   * \code
   * auto surf_mesh_ptr = Chi::GetStackItemPtr(
      Chi::object_stack, surf_mesh_hndle, fname);
     // Returns std::shared_ptr<ChiObject>
   * \endcode
   * */
  template <class T>
  static std::shared_ptr<T>& GetStackItemPtr(std::vector<std::shared_ptr<T>>& stack,
                                             const size_t handle,
                                             const std::string& calling_function_name = "Unknown")
  {
    try
    {
      std::shared_ptr<T>& item = stack.at(handle);
      return item;
    }
    catch (const std::out_of_range& oor)
    {
      throw std::out_of_range("chi::GetStackItem: Invalid handle used. "
                              "Calling function: " +
                              calling_function_name);
    }
  }
};
