#pragma once

#include <QVector>
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <score/command/Command.hpp>
#include <score/model/path/Path.hpp>
#include <score/tools/std/Optional.hpp>

#include <score/model/Identifier.hpp>

struct DataStreamInput;
struct DataStreamOutput;

namespace Scenario
{
class EventModel;
class TimeSyncModel;
namespace Command
{
class SplitTimeSync final : public score::Command
{
  SCORE_COMMAND_DECL(
      ScenarioCommandFactoryName(), SplitTimeSync, "Desynchronize")
public:
  SplitTimeSync(
      const TimeSyncModel& path, QVector<Id<EventModel>> eventsInNewTimeSync);
  void undo(const score::DocumentContext& ctx) const override;
  void redo(const score::DocumentContext& ctx) const override;

protected:
  void serializeImpl(DataStreamInput&) const override;
  void deserializeImpl(DataStreamOutput&) override;

private:
  Path<TimeSyncModel> m_path;
  QVector<Id<EventModel>> m_eventsInNewTimeSync;

  Id<TimeSyncModel> m_originalTimeSyncId;
  Id<TimeSyncModel> m_newTimeSyncId;
};
}
}
