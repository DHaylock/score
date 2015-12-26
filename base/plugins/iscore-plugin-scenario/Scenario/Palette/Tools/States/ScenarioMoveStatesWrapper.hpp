#pragma once
#include <Scenario/Palette/Tools/States/MoveStates.hpp>
#include <Scenario/Commands/Scenario/Displacement/MoveConstraint.hpp>
#include <Scenario/Commands/Scenario/Displacement/MoveEventMeta.hpp>

#include <Scenario/Palette/Transitions/ConstraintTransitions.hpp>
#include <Scenario/Palette/Transitions/EventTransitions.hpp>
#include <Scenario/Palette/Transitions/StateTransitions.hpp>
#include <Scenario/Palette/Transitions/TimeNodeTransitions.hpp>
namespace Scenario
{
class MoveConstraintInScenario_StateWrapper
{
    public:
        template<
                typename Scenario_T,
                typename ToolPalette_T>
        static void make(const ToolPalette_T& palette, QState* waitState, QState& parent)
        {
            /// Constraint
            /// //TODO remove useless arguments to ctor
            auto moveConstraint =
                    new MoveConstraintState<MoveConstraint, Scenario_T, ToolPalette_T>{
                        palette,
                        palette.model(),
                        palette.context().commandStack,
                        palette.context().objectLocker,
                        &parent};

            iscore::make_transition<ClickOnConstraint_Transition<Scenario_T>>(waitState,
                                                          moveConstraint,
                                                          *moveConstraint);
            moveConstraint->addTransition(moveConstraint,
                                          finishedState(),
                                          waitState);
        }
};

class MoveEventInScenario_StateWrapper
{
    public:
        template<
                typename Scenario_T,
                typename ToolPalette_T>
        static void make(const ToolPalette_T& palette, QState* waitState, QState& parent)
        {
            /// Event
            auto moveEvent =
                    new MoveEventState<MoveEventMeta, Scenario_T, ToolPalette_T>{
                        palette,
                        palette.model(),
                        palette.context().commandStack,
                        palette.context().objectLocker,
                        &parent};

            iscore::make_transition<ClickOnState_Transition<Scenario_T>>(waitState,
                                                     moveEvent,
                                                     *moveEvent);

            iscore::make_transition<ClickOnEvent_Transition<Scenario_T>>(waitState,
                                                     moveEvent,
                                                     *moveEvent);
            moveEvent->addTransition(moveEvent,
                                       finishedState(),
                                       waitState);
        }
};

class MoveTimeNodeInScenario_StateWrapper
{
    public:
        template<
                typename Scenario_T,
                typename ToolPalette_T>
        static void make(const ToolPalette_T& palette, QState* waitState, QState& parent)
        {
            /// TimeNode
            auto moveTimeNode =
                    new MoveTimeNodeState<MoveEventMeta, Scenario_T, ToolPalette_T>{
                palette,
                        palette.model(),
                        palette.context().commandStack,
                        palette.context().objectLocker,
                        &parent};

            iscore::make_transition<ClickOnTimeNode_Transition<Scenario_T>>(waitState,
                                                                    moveTimeNode,
                                                                    *moveTimeNode);
            moveTimeNode->addTransition(moveTimeNode,
                                        finishedState(),
                                        waitState);
        }
};
}
