#include "framework/event_system/system_wide_event_publisher.h"

namespace opensn
{

SystemWideEventPublisher::SystemWideEventPublisher(opensn::App& app)
  : EventPublisher(app, "SystemWide")
{
}

void
SystemWideEventPublisher::PublishEvent(const Event& event)
{
  EventPublisher::PublishEvent(event);
}

} // namespace opensn
