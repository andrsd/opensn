#pragma once

#include "framework/event_system/event_publisher.h"

namespace opensn
{

class SystemWideEventPublisher : public EventPublisher
{
public:
  static SystemWideEventPublisher& GetInstance();

  /// Deleted copy constructor
  SystemWideEventPublisher(const SystemWideEventPublisher&) = delete;
  SystemWideEventPublisher
  /// Deleted assignment operator
  operator=(const SystemWideEventPublisher&) = delete;

  void PublishEvent(const Event& event) override;

private:
  SystemWideEventPublisher();
};

} // namespace opensn
