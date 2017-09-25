#pragma once
#include <QJsonObject>
#include <QMap>
#include <QVector>
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <score/command/Command.hpp>
#include <score/tools/std/Optional.hpp>

#include <score/model/path/Path.hpp>
#include <score/model/Identifier.hpp>

struct DataStreamInput;
struct DataStreamOutput;

namespace Scenario
{
struct Point;
class EventModel;
class StateModel;
class SynchronizationModel;
class IntervalModel;
namespace Command
{

class ScenarioPasteElements final : public score::Command
{
  SCORE_COMMAND_DECL(
      ScenarioCommandFactoryName(),
      ScenarioPasteElements,
      "Paste elements in scenario")
public:
  ScenarioPasteElements(
      const Scenario::ProcessModel& path,
      const QJsonObject& obj,
      const Scenario::Point& pt);

  void undo(const score::DocumentContext& ctx) const override;
  void redo(const score::DocumentContext& ctx) const override;

protected:
  void serializeImpl(DataStreamInput&) const override;
  void deserializeImpl(DataStreamOutput&) override;

private:
  Path<Scenario::ProcessModel> m_ts;

  // TODO std::vector...
  QVector<Id<SynchronizationModel>> m_ids_timesyncs;
  QVector<Id<IntervalModel>> m_ids_intervals;
  QVector<Id<EventModel>> m_ids_events;
  QVector<Id<StateModel>> m_ids_states;

  QVector<QJsonObject> m_json_timesyncs;
  QVector<QJsonObject> m_json_intervals;
  QVector<QJsonObject> m_json_events;
  QVector<QJsonObject> m_json_states;
};
}
}
