// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#include "framework/event_system/event.h"
#include <map>

namespace opensn
{

Event::Event(const std::string& name) : name_(name), code_(Event::GetStandardCode(name)), params_()
{
}

Event::Event(const std::string& name, const ParameterBlock& parameter_block)
  : name_(name), code_(Event::GetStandardCode(name)), params_(parameter_block)
{
}

const std::string&
Event::GetName() const
{
  return name_;
}

Event::EventCode
Event::GetCode() const
{
  return code_;
}

const ParameterBlock&
Event::Parameters() const
{
  return params_;
}

bool
Event::IsSolverEvent() const
{
  return (code_ >= SolverPreInitialize) and (code_ <= SolverAdvanced);
}

Event::EventCode
Event::GetStandardCode(const std::string& event_name)
{
  static std::map<std::string, Event::EventCode> event_name_2_code_map{
    {"ProgramStart", Event::ProgramStart},
    {"ProgramExecuted", Event::ProgramExecuted},
    {"SolverPreInitialize", Event::SolverPreInitialize},
    {"SolverInitialized", Event::SolverInitialized},
    {"SolverPreExecution", Event::SolverPreExecution},
    {"SolverExecuted", Event::SolverExecuted},
    {"SolverPreStep", Event::SolverPreStep},
    {"SolverStep", Event::SolverStep},
    {"SolverPreAdvance", Event::SolverPreAdvance},
    {"SolverAdvanced", Event::SolverAdvanced},
  };

  const auto it = event_name_2_code_map.find(event_name);
  if (it != event_name_2_code_map.end())
    return it->second;

  return Event::Unknown;
}

} // namespace opensn
