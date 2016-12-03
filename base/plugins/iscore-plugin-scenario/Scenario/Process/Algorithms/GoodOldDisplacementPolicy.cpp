#include <QByteArray>
#include <QMap>
#include <QPair>
#include <iscore/tools/std/Optional.hpp>

#include "GoodOldDisplacementPolicy.hpp"
#include <Scenario/Document/Constraint/ConstraintDurations.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/Constraint/ViewModels/ConstraintViewModel.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>
#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <Scenario/Tools/dataStructures.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/tools/EntityMap.hpp>
#include <iscore/tools/ModelPath.hpp>
#include <iscore/tools/SettableIdentifier.hpp>

template <typename T>
class Reader;

namespace Scenario
{
class RackModel;
void GoodOldDisplacementPolicy::computeDisplacement(
    Scenario::ProcessModel& scenario,
    const QVector<Id<TimeNodeModel>>& draggedElements,
    const TimeValue& deltaTime,
    ElementsProperties& elementsProperties)
{
  // this old behavior supports only the move of one timenode
  if (draggedElements.length() != 1)
  {
    qDebug()
        << "WARNING : computeDisplacement called with empty element list !";
    // move nothing, nothing to undo or redo
    return;
  }
  else
  {
    const Id<TimeNodeModel>& firstTimeNodeMovedId = draggedElements.at(0);
    std::vector<Id<TimeNodeModel>> timeNodesToTranslate;

    GoodOldDisplacementPolicy::getRelatedTimeNodes(
        scenario, firstTimeNodeMovedId, timeNodesToTranslate);

    // put each concerned timenode in modified elements and compute new values
    for (const auto& curTimeNodeId : timeNodesToTranslate)
    {
      auto& curTimeNode = scenario.timeNodes.at(curTimeNodeId);

      // if timenode NOT already in element properties, create new element
      // properties and set the old date
      auto tn_it = elementsProperties.timenodes.find(curTimeNodeId);
      if (tn_it == elementsProperties.timenodes.end())
      {
        TimenodeProperties t;
        t.oldDate = curTimeNode.date();
        tn_it = elementsProperties.timenodes.insert(curTimeNodeId, t);
      }

      // put the new date
      tn_it->newDate = tn_it->oldDate + deltaTime;
    }

    // Make a list of the constraints that need to be resized
    for (const auto& curTimeNodeId : timeNodesToTranslate)
    {
      auto& curTimeNode = scenario.timeNode(curTimeNodeId);

      // each previous constraint
      for (const auto& ev_id : curTimeNode.events())
      {
        const auto& ev = scenario.event(ev_id);
        for (const auto& st_id : ev.states())
        {
          const auto& st = scenario.states.at(st_id);
          if (const auto& optCurConstraintId = st.previousConstraint())
          {
            auto curConstraintId = *optCurConstraintId;
            auto& curConstraint = scenario.constraints.at(curConstraintId);

            // if timenode NOT already in element properties, create new
            // element properties and set old values
            auto cur_constraint_it
                = elementsProperties.constraints.find(curConstraintId);
            if (cur_constraint_it == elementsProperties.constraints.end())
            {
              ConstraintProperties c{curConstraint};
              c.oldMin = curConstraint.duration.minDuration();
              c.oldMax = curConstraint.duration.maxDuration();
              cur_constraint_it
                  = elementsProperties.constraints.insert(curConstraintId, c);
            }

            auto& curConstraintStartEvent
                = Scenario::startEvent(curConstraint, scenario);
            auto& startTnodeId = curConstraintStartEvent.timeNode();

            // compute default duration
            TimeValue startDate;

            // if prev tnode has moved take updated value else take existing
            auto it = elementsProperties.timenodes.constFind(startTnodeId);
            if (it != elementsProperties.timenodes.constEnd())
            {
              startDate = it->newDate;
            }
            else
            {
              startDate = curConstraintStartEvent.date();
            }

            const auto& endDate
                = elementsProperties.timenodes[curTimeNodeId].newDate;

            TimeValue newDefaultDuration = endDate - startDate;
            TimeValue deltaBounds = newDefaultDuration
                                    - curConstraint.duration.defaultDuration();

            cur_constraint_it->newMin
                = curConstraint.duration.minDuration() + deltaBounds;
            cur_constraint_it->newMax
                = curConstraint.duration.maxDuration() + deltaBounds;

            // nothing to do for now
          }
        }
      }
    }
  }
}

void GoodOldDisplacementPolicy::getRelatedTimeNodes(
    Scenario::ProcessModel& scenario,
    const Id<TimeNodeModel>& firstTimeNodeMovedId,
    std::vector<Id<TimeNodeModel>>& translatedTimeNodes)
{
  if (firstTimeNodeMovedId.val() == Scenario::startId_val()
      || firstTimeNodeMovedId.val() == Scenario::endId_val())
    return;

  auto it = std::find(
      translatedTimeNodes.begin(),
      translatedTimeNodes.end(),
      firstTimeNodeMovedId);
  if (it == translatedTimeNodes.end())
  {
    translatedTimeNodes.push_back(firstTimeNodeMovedId);
  }
  else // timeNode already moved
  {
    return;
  }

  const auto& cur_timeNode = scenario.timeNodes.at(firstTimeNodeMovedId);
  for (const auto& cur_eventId : cur_timeNode.events())
  {
    const auto& cur_event = scenario.events.at(cur_eventId);

    for (const auto& state_id : cur_event.states())
    {
      const auto& state = scenario.states.at(state_id);
      if (const auto& cons = state.nextConstraint())
      {
        const auto& endStateId = scenario.constraints.at(*cons).endState();
        const auto& endTnId
            = scenario.events.at(scenario.state(endStateId).eventId())
                  .timeNode();
        getRelatedTimeNodes(scenario, endTnId, translatedTimeNodes);
      }
    }
  }
}
}
