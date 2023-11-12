#include "framework/math/time_integrations/implicit_euler_time_intgr.h"

#include "framework/object_factory.h"

#define scint static_cast<int>

namespace chi_math
{

RegisterChiObject(chi_math, ImplicitEulerTimeIntegration);

chi::InputParameters
ImplicitEulerTimeIntegration::GetInputParameters()
{
  chi::InputParameters params = ThetaSchemeTimeIntegration::GetInputParameters();

  // clang-format off
  params.SetGeneralDescription("General implicit Euler Time Integration");
  params.SetDocGroup("DocTimeIntegrations");
  // clang-format on

  params.ChangeExistingParamToOptional("method", scint(SteppingMethod::IMPLICIT_EULER));
  params.ChangeExistingParamToOptional("theta", 1.0);

  return params;
}

ImplicitEulerTimeIntegration::ImplicitEulerTimeIntegration(opensn::App& app,
                                                           const chi::InputParameters& params)
  : ThetaSchemeTimeIntegration(app, params)
{
}

} // namespace chi_math
