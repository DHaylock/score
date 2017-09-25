#pragma once
#include <Inspector/InspectorWidgetBase.hpp>
#include <list>
#include <set>
#include <vector>

namespace Scenario
{
class IntervalModel;
class SynchronizationModel;
class EventModel;
class StateModel;
class SummaryInspectorWidget final : public Inspector::InspectorWidgetBase
{
public:
  SummaryInspectorWidget(
      const IdentifiedObjectAbstract* obj,
      const std::set<const IntervalModel*>&
          intervals,
      const std::set<const SynchronizationModel*>&
          timesyncs,
      const std::set<const EventModel*>&
          events,
      const std::set<const StateModel*>&
          states,
      const score::DocumentContext& context,
      QWidget* parent = nullptr);

  QString tabName() override;

private:
  std::vector<QWidget*> m_properties;
};
}
