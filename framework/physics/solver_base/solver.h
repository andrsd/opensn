#pragma once

#include "framework/object.h"
#include "framework/physics/physics_namespace.h"

#include "framework/physics/basic_options/basic_options.h"
#include "framework/parameters/parameter_block.h"

#include <iostream>
#include <utility>

namespace chi_physics
{
class FieldFunctionGridBased;
class TimeStepper;

/**\defgroup SolverBase Base class for all solvers
 * \ingroup doc_PhysicsSolver*/
class Solver : public chi::ChiObject
{
public:
  /**Returns the input parameters.*/
  static chi::InputParameters GetInputParameters();
  explicit Solver(opensn::App& app, std::string in_text_name);
  Solver(opensn::App& app, std::string in_text_name, std::initializer_list<BasicOption> in_options);
  explicit Solver(opensn::App& app, const chi::InputParameters& params);
  virtual ~Solver() = default;

  std::string TextName() const;

  BasicOptions& GetBasicOptions();
  const BasicOptions& GetBasicOptions() const;

  std::vector<std::shared_ptr<FieldFunctionGridBased>>& GetFieldFunctions();

  const std::vector<std::shared_ptr<FieldFunctionGridBased>>& GetFieldFunctions() const;

  TimeStepper& GetTimeStepper();
  const TimeStepper& GetTimeStepper() const;

  /**Initialize function.*/
  virtual void Initialize();
  /**Execution function.*/
  virtual void Execute();
  /**Step function*/
  virtual void Step();
  /**Advance time values function.*/
  virtual void Advance();

  /**Generalized query for information supporting varying returns.*/
  virtual chi::ParameterBlock GetInfo(const chi::ParameterBlock& params) const;
  /**\addtogroup SolverBase
   *
   * \section Properties Properties that can be set
   * The following properties can be set via the lua call
   * `chi_lua::chiSolverSetProperties`
   * \copydoc chi_physics::Solver::SetProperties
   *
   * Base solver settable properties:
   * - `dt`, Timestep size
   * - `time`, Current time
   */
  virtual void SetProperties(const chi::ParameterBlock& params);
  /**PreCheck call to GetInfo.*/
  chi::ParameterBlock GetInfoWithPreCheck(const chi::ParameterBlock& params) const;

protected:
  BasicOptions basic_options_;
  std::vector<std::shared_ptr<FieldFunctionGridBased>> field_functions_;
  std::shared_ptr<TimeStepper> timestepper_ = nullptr;

private:
  static std::shared_ptr<TimeStepper> InitTimeStepper(opensn::App& app,
                                                      const chi::InputParameters& params);
  const std::string text_name_;
};

} // namespace chi_physics
