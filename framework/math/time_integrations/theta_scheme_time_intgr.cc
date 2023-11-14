#include "framework/math/time_integrations/theta_scheme_time_intgr.h"

#include "framework/object_factory.h"

#define scint static_cast<int>

namespace chi_math
{

RegisterChiObject(chi_math, ThetaSchemeTimeIntegration);

chi::InputParameters
ThetaSchemeTimeIntegration::GetInputParameters()
{
  chi::InputParameters params = TimeIntegration::GetInputParameters();

  // clang-format off
  params.SetGeneralDescription("Generalized theta-scheme");
  params.SetDocGroup("DocTimeIntegrations");
  // clang-format on

  params.ChangeExistingParamToOptional("method", scint(SteppingMethod::THETA_SCHEME));

  params.AddRequiredParameter<double>("theta", "The theta parameter for a theta scheme");

  return params;
}

ThetaSchemeTimeIntegration::ThetaSchemeTimeIntegration(opensn::App& app,
                                                       const chi::InputParameters& params)
  : TimeIntegration(app, params), theta_(params.GetParamValue<double>("theta"))
{
}

double
ThetaSchemeTimeIntegration::ThetaFactor() const
{
  return theta_;
}

} // namespace chi_math
