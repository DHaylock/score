// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QByteArray>
#include <Scenario/Commands/Synchronization/AddTrigger.hpp>
#include <Scenario/Commands/Synchronization/RemoveTrigger.hpp>
#include <Scenario/Document/Synchronization/SynchronizationModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>

#include "ScenarioTriggerCommandFactory.hpp"
#include <Scenario/Process/ScenarioInterface.hpp>
#include <score/command/Command.hpp>
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/model/path/Path.hpp>
#include <score/model/path/PathSerialization.hpp>
namespace Scenario
{
namespace Command
{

bool ScenarioTriggerCommandFactory::matches(const SynchronizationModel& tn) const
{
  return dynamic_cast<Scenario::ProcessModel*>(tn.parent());
}

score::Command*
ScenarioTriggerCommandFactory::make_addTriggerCommand(
    const SynchronizationModel& tn) const
{
  if (dynamic_cast<Scenario::ProcessModel*>(tn.parent()))
  {
    return new Scenario::Command::AddTrigger<Scenario::ProcessModel>(tn);
  }
  return nullptr;
}

score::Command*
ScenarioTriggerCommandFactory::make_removeTriggerCommand(
    const SynchronizationModel& tn) const
{
  if (dynamic_cast<Scenario::ProcessModel*>(tn.parent()))
  {
    return new Scenario::Command::RemoveTrigger<Scenario::ProcessModel>(tn);
  }
  return nullptr;
}
}
}
