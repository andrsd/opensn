#pragma once

#include "framework/object.h"
#include "framework/event_system/event_subscriber.h"

namespace chi
{

enum class PPType : int
{
  NO_VALUE = 0,
  SCALAR = 1,
  VECTOR = 2,
  ARBITRARY = 3
};

enum class PPNumericFormat : int
{
  FIXED = 0,
  FLOATING_POINT = 1,
  SCIENTIFIC = 2,
  GENERAL = 3,
};

/**Base class for all post-processors.*/
class PostProcessor : public ChiObject, public EventSubscriber
{
public:
  struct TimeHistoryEntry
  {
    size_t t_index_;
    double time_;
    ParameterBlock value_;
  };

  /**Returns the name of the post-processors.*/
  const std::string& Name() const;
  /**Returns the type of the post-processors. This is the generic type
   * SCALAR, VECTOR, etc. not the c++ type.*/
  PPType Type() const;

  /**Returns the numeric format of the post-processor for printing.*/
  PPNumericFormat NumericFormat() const;
  /**Returns the numeric precision of the post-processor for printing.*/
  size_t NumericPrecision() const;

  /**Calls the base ChiObject's method and adds a subscription to
   * `chi_physics::PhysicsEventPublisher` singleton.*/
  void PushOntoStack(std::shared_ptr<ChiObject>& new_object) override;

  void ReceiveEventUpdate(const Event& event) override;

  virtual void Execute(const Event& event_context) = 0;

  /**Gets the scalar value currently stored for the post-processor.*/
  virtual const ParameterBlock& GetValue() const;
  virtual const std::vector<TimeHistoryEntry>& GetTimeHistory() const;

  const std::vector<std::string>& PrintScope() const;

  /**Converts a scalar value into a string format based on this post-processor's
   * numeric specifications.*/
  std::string ConvertScalarValueToString(const ParameterBlock& value) const;

  /**Converts a scalar and vector values into a string format based on this
   * post-processor's numeric specifications.*/
  std::string ConvertValueToString(const ParameterBlock& value) const;

  virtual ~PostProcessor() = default;

  static InputParameters GetInputParameters();

protected:
  explicit PostProcessor(opensn::App& app, const InputParameters& params, PPType type);

  static PPType FigureTypeFromValue(const ParameterBlock& value);

  /**Sets the post-processor's generic type.*/
  void SetType(PPType type);

  const std::string name_;
  std::vector<std::string> subscribed_events_for_execution_;
  std::vector<std::string> subscribed_events_for_printing_;
  PPType type_;
  ParameterBlock value_;

  std::vector<TimeHistoryEntry> time_history_;

  const PPNumericFormat print_numeric_format_ = PPNumericFormat::GENERAL;
  const size_t print_precision_ = 6;
  std::string solvername_filter_;

private:
  static PPNumericFormat ConstructNumericFormat(const std::string& format_string);
};

typedef std::shared_ptr<PostProcessor> PostProcessorPtr;

} // namespace chi
