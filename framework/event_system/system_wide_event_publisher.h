#pragma once

#include "framework/event_system/event_publisher.h"

namespace opensn
{

class SystemWideEventPublisher : public EventPublisher
{
public:
  explicit SystemWideEventPublisher(opensn::App& app);
  /// Deleted copy constructor
  SystemWideEventPublisher(const SystemWideEventPublisher&) = delete;
  /// Deleted assignment operator
  SystemWideEventPublisher operator=(const SystemWideEventPublisher&) = delete;

  void PublishEvent(const Event& event) override;
};

} // namespace opensn
