#pragma once

#include <vector>
#include <memory>
#include <string>

namespace opensn
{
class App;
class Event;
class EventSubscriber;

/**
 * Base class for event publishers.
 */
class EventPublisher
{
protected:
  EventPublisher(opensn::App& app, const std::string& name);

public:
  /**
   * Publish the given event.
   */
  virtual void PublishEvent(const Event& event);

  /**
   * Adds a subscriber to the publisher.
   */
  void AddSubscriber(std::shared_ptr<EventSubscriber>& subscriber_sptr);

  virtual ~EventPublisher() = default;

  opensn::App& App() const { return app_; }

protected:
  opensn::App& app_;
  const std::string publisher_name_;
  std::vector<std::weak_ptr<EventSubscriber>> subscribers_;
};

} // namespace opensn
