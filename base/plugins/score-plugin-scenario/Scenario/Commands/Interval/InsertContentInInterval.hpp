#pragma once
#include <Process/ExpandMode.hpp>
#include <QJsonObject>
#include <QMap>
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <score/command/Command.hpp>
#include <score/model/path/Path.hpp>

#include <score/model/Identifier.hpp>

struct DataStreamInput;
struct DataStreamOutput;
namespace Process
{
class ProcessModel;
}
namespace Scenario
{
class IntervalModel;

namespace Command
{
class SCORE_PLUGIN_SCENARIO_EXPORT InsertContentInInterval final : public score::Command
{
  SCORE_COMMAND_DECL(
      ScenarioCommandFactoryName(),
      InsertContentInInterval,
      "Insert content in a interval")
public:
  InsertContentInInterval(
      QJsonObject&& sourceInterval,
      const IntervalModel& targetInterval,
      ExpandMode mode);

  void undo(const score::DocumentContext& ctx) const override;
  void redo(const score::DocumentContext& ctx) const override;

protected:
  void serializeImpl(DataStreamInput&) const override;
  void deserializeImpl(DataStreamOutput&) override;

private:
  QJsonObject m_source;
  Path<IntervalModel> m_target;
  ExpandMode m_mode{ExpandMode::GrowShrink};

  QMap<Id<Process::ProcessModel>, Id<Process::ProcessModel>> m_processIds;
};
}
}
