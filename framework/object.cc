#include "framework/object.h"
#include "framework/app.h"

namespace chi
{

static const size_t SIZE_T_INVALID = ((size_t)-1);

InputParameters
ChiObject::GetInputParameters()
{
  InputParameters params;
  //  params.AddPrivateParameter<opensn::App*>("_app", nullptr, "Application this object belongs
  //  to");
  return params;
}

ChiObject::ChiObject(opensn::App& app) : stack_id_(SIZE_T_INVALID), app_(app)
{
}

ChiObject::ChiObject(opensn::App& app, const chi::InputParameters& params)
  : stack_id_(SIZE_T_INVALID), app_(app)
{
}

void
ChiObject::SetStackID(size_t stack_id)
{
  stack_id_ = stack_id;
}

size_t
ChiObject::StackID() const
{
  return stack_id_;
}

opensn::App&
ChiObject::App() const
{
  return app_;
}

void
ChiObject::PushOntoStack(std::shared_ptr<ChiObject>& new_object)
{
  app_.ObjectStack().push_back(new_object);
  new_object->SetStackID(app_.ObjectStack().size() - 1);
}

} // namespace chi
