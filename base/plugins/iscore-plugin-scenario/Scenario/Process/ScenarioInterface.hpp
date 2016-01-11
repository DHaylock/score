#pragma once
#include <iscore/tools/SettableIdentifier.hpp>
#include <iscore_plugin_scenario_export.h>
namespace Scenario
{
class ConstraintModel;
class EventModel;
class StateModel;
class TimeNodeModel;
class ISCORE_PLUGIN_SCENARIO_EXPORT ScenarioInterface
{
    public:
        virtual ~ScenarioInterface();
        virtual ConstraintModel* findConstraint(const Id<ConstraintModel>& constraintId) const = 0;
        virtual EventModel* findEvent(const Id<EventModel>& eventId) const = 0;
        virtual TimeNodeModel* findTimeNode(const Id<TimeNodeModel>& timeNodeId) const = 0;
        virtual StateModel* findState(const Id<StateModel>& stId) const = 0;

        virtual ConstraintModel& constraint(const Id<ConstraintModel>& constraintId) const = 0;
        virtual EventModel& event(const Id<EventModel>& eventId) const = 0;
        virtual TimeNodeModel& timeNode(const Id<TimeNodeModel>& timeNodeId) const = 0;
        virtual StateModel& state(const Id<StateModel>& stId) const = 0;

        virtual TimeNodeModel& startTimeNode() const = 0;
        virtual TimeNodeModel& endTimeNode() const = 0;
};

template<typename T>
static auto startId()
{
    return Id<T>{0};
}
template<typename T>
static auto endId()
{
    return Id<T>{1};
}
}
