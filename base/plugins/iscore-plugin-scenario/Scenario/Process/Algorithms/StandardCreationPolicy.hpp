#pragma once
#include <Process/TimeValue.hpp>
#include <iscore/tools/SettableIdentifier.hpp>



namespace Scenario
{
class ConstraintModel;
class ConstraintViewModel;
class EventModel;
class StateModel;
class CommentBlockModel;

class TimeNodeModel;
class ScenarioModel;
struct VerticalExtent;
template<typename T>
class ScenarioCreate;
template<>
class ScenarioCreate<CommentBlockModel>
{
    public:
        static void undo(
                const Id<CommentBlockModel>& id,
                Scenario::ScenarioModel& s);

        static CommentBlockModel& redo(
                const Id<CommentBlockModel>& id,
                const TimeValue& date,
                double y,
                Scenario::ScenarioModel& s);
};

template<>
class ScenarioCreate<TimeNodeModel>
{
    public:
        static void undo(
                const Id<TimeNodeModel>& id,
                Scenario::ScenarioModel& s);

        static TimeNodeModel& redo(
                const Id<TimeNodeModel>& id,
                const VerticalExtent& extent,
                const TimeValue& date,
                Scenario::ScenarioModel& s);
};

template<>
class ScenarioCreate<EventModel>
{
    public:
        static void undo(
                const Id<EventModel>& id,
                Scenario::ScenarioModel& s);

        static EventModel& redo(
                const Id<EventModel>& id,
                TimeNodeModel& timenode,
                const VerticalExtent& extent,
                Scenario::ScenarioModel& s);
};

template<>
class ScenarioCreate<StateModel>
{
    public:
        static void undo(
                const Id<StateModel>& id,
                Scenario::ScenarioModel& s);

        static StateModel& redo(
                const Id<StateModel>& id,
                EventModel& ev,
                double y,
                Scenario::ScenarioModel& s);
};

template<>
class ScenarioCreate<ConstraintModel>
{
    public:
        static void undo(
                const Id<ConstraintModel>& id,
                Scenario::ScenarioModel& s);

        static ConstraintModel& redo(
                const Id<ConstraintModel>& id,
                const Id<ConstraintViewModel>& fullviewid,
                StateModel& sst,
                StateModel& est,
                double ypos,
                Scenario::ScenarioModel& s);
};

}  // namespace Scenario

