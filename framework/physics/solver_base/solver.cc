#include "framework/physics/solver_base/solver.h"
#include "framework/app.h"
#include "framework/logging/log.h"
#include "framework/physics/time_steppers/constant_time_stepper.h"

namespace chi_physics
{

chi::InputParameters
Solver::GetInputParameters()
{
  chi::InputParameters params = ChiObject::GetInputParameters();

  params.AddRequiredParameter<std::string>(
    "name",
    "A text name to associate with the solver. This name will be used "
    "in status messages and verbose iterative convergence monitors.");

  params.AddOptionalParameter("dt", 0.01, "Desired initial timestep size.");
  params.AddOptionalParameter("time", 0.0, "Current time of the solver.");
  params.AddOptionalParameter("start_time", 0.0, "Transient start-time if applicable.");
  params.AddOptionalParameter("end_time", 1.0, "Transient end-time if applicable.");
  params.AddOptionalParameter(
    "max_time_steps", -1, "Maximum number of timesteps to allow. Negative values disables this.");

  params.AddOptionalParameter("timestepper",
                              0,
                              "Handle to a timestepper. If not supplied then a ConstantTimeStepper "
                              "will be created.");

  using namespace chi_data_types;
  params.ConstrainParameterRange("dt", AllowableRangeLowLimit::New(1.0e-12));

  return params;
}

Solver::Solver(opensn::App& app, std::string in_text_name)
  : chi::ChiObject(app),
    timestepper_(InitTimeStepper(app, GetInputParameters())),
    text_name_(std::move(in_text_name))
{
}

Solver::Solver(opensn::App& app,
               std::string in_text_name,
               std::initializer_list<BasicOption> in_options)
  : chi::ChiObject(app),
    basic_options_(in_options),
    timestepper_(InitTimeStepper(app, GetInputParameters())),
    text_name_(std::move(in_text_name))
{
}

Solver::Solver(opensn::App& app, const chi::InputParameters& params)
  : ChiObject(app, params),
    timestepper_(InitTimeStepper(app, params)),
    text_name_(params.GetParamValue<std::string>("name"))
{
}

std::shared_ptr<TimeStepper>
Solver::InitTimeStepper(opensn::App& app, const chi::InputParameters& params)
{
  const auto& user_params = params.ParametersAtAssignment();

  if (user_params.Has("timestepper"))
  {
    auto stepper =
      app.GetStackObject<TimeStepper>(params.GetParamValue<size_t>("timestepper"), __FUNCTION__);

    stepper->SetTimeStepSize(params.GetParamValue<double>("dt"));
    stepper->SetTime(params.GetParamValue<double>("time"));
    stepper->SetStartTime(params.GetParamValue<double>("start_time"));
    stepper->SetEndTime(params.GetParamValue<double>("end_time"));
    stepper->SetMaxTimeSteps(params.GetParamValue<int>("max_time_steps"));

    return stepper;
  }
  else
  {
    auto& factory = app.Factory();

    const std::string obj_type = "chi_physics::ConstantTimeStepper";
    auto valid_params = ChiObjectFactory::GetRegisteredObjectParameters(obj_type);
    chi::ParameterBlock custom_params;

    if (params.NumParameters() != 0)
    {
      custom_params.AddParameter(params.GetParam("dt"));
      custom_params.AddParameter(params.GetParam("time"));
      custom_params.AddParameter(params.GetParam("start_time"));
      custom_params.AddParameter(params.GetParam("end_time"));
      custom_params.AddParameter(params.GetParam("max_time_steps"));
    }

    valid_params.AssignParameters(custom_params);

    auto stepper = std::make_shared<ConstantTimeStepper>(app, valid_params);
    app.ObjectStack().push_back(stepper);
    stepper->SetStackID(app.ObjectStack().size() - 1);

    return stepper;
  }
}

std::string
Solver::TextName() const
{
  return text_name_;
}

BasicOptions&
Solver::GetBasicOptions()
{
  return basic_options_;
}

const BasicOptions&
Solver::GetBasicOptions() const
{
  return basic_options_;
}

std::vector<std::shared_ptr<FieldFunctionGridBased>>&
Solver::GetFieldFunctions()
{
  return field_functions_;
}

TimeStepper&
Solver::GetTimeStepper()
{
  ChiLogicalErrorIf(not timestepper_, "Bad trouble: Timestepper not assigned.");
  return *timestepper_;
}

const TimeStepper&
Solver::GetTimeStepper() const
{
  ChiLogicalErrorIf(not timestepper_, "Bad trouble: Timestepper not assigned.");
  return *timestepper_;
}

const std::vector<std::shared_ptr<FieldFunctionGridBased>>&
Solver::GetFieldFunctions() const
{
  return field_functions_;
}

void
Solver::Initialize()
{
  App().Log().Log() << "\"Initialize()\" method not defined for " << TextName();
}

void
Solver::Execute()
{
  App().Log().Log() << "\"Execute()\" method not defined for " << TextName();
}

void
Solver::Step()
{
  App().Log().Log() << "\"Step()\" method not defined for " << TextName();
}

void
Solver::Advance()
{
  App().Log().Log() << "\"Advance()\" method not defined for " << TextName();
}

chi::ParameterBlock
Solver::GetInfo(const chi::ParameterBlock& params) const
{
  return chi::ParameterBlock{};
}

chi::ParameterBlock
Solver::GetInfoWithPreCheck(const chi::ParameterBlock& params) const
{
  if (not params.Has("name"))
  {
    App().Log().LogAllWarning() << "chi_physics::Solver::GetInfo called without "
                                   "\"name\" in the parameter list";
    return chi::ParameterBlock{};
  }
  return GetInfo(params);
}

void
Solver::SetProperties(const chi::ParameterBlock& params)
{
  for (const auto& param : params)
  {
    const std::string param_name = param.Name();

    if (param_name == "dt") timestepper_->SetTimeStepSize(param.GetValue<double>());
    if (param_name == "time") timestepper_->SetTime(param.GetValue<double>());
    if (param_name == "start_time") timestepper_->SetStartTime(param.GetValue<double>());
    if (param_name == "end_time") timestepper_->SetEndTime(param.GetValue<double>());
    if (param_name == "max_time_steps") timestepper_->SetMaxTimeSteps(param.GetValue<int>());
    if (param_name == "dt_min") timestepper_->SetMinimumTimeStepSize(param.GetValue<int>());
  }
}

} // namespace chi_physics
