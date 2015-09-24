#include "StateModel.hpp"

#include "StateView.hpp"

#include "Document/Constraint/ViewModels/ConstraintView.hpp"
#include "Document/Event/EventModel.hpp"

#include "Process/ScenarioModel.hpp"
#include "Process/Temporal/TemporalScenarioPresenter.hpp"
#include <ProcessInterface/State/ProcessStateDataInterface.hpp>
#include <iscore/document/DocumentInterface.hpp>

StateModel::StateModel(
        const Id<StateModel>& id,
        const Id<EventModel>& eventId,
        double yPos,
        QObject *parent):
    IdentifiedObject<StateModel> {id, "StateModel", parent},
    m_eventId{eventId},
    m_heightPercentage{yPos},
    m_messageItemModel{new iscore::MessageItemModel{
                            iscore::IDocument::commandStack(*this),
                            this}}
{
    init();
}

StateModel::StateModel(
        const StateModel &source,
        const Id<StateModel> &id,
        QObject *parent):
    StateModel{id, source.eventId(), source.heightPercentage(), parent}
{
    messages() = source.messages();
    init();
}

void StateModel::init()
{
    con(m_messageItemModel, &QAbstractItemModel::modelReset,
        this, &StateModel::statesUpdated_slt);
    con(m_messageItemModel, &QAbstractItemModel::dataChanged,
        this, &StateModel::statesUpdated_slt);
    con(m_messageItemModel, &QAbstractItemModel::rowsInserted,
        this, &StateModel::statesUpdated_slt);
    con(m_messageItemModel, &QAbstractItemModel::rowsMoved,
        this, &StateModel::statesUpdated_slt);
    con(m_messageItemModel, &QAbstractItemModel::rowsRemoved,
        this, &StateModel::statesUpdated_slt);

    if(m_previousConstraint)
    {
        const auto& cstr = parentScenario()->constraint(m_previousConstraint);
        for(const auto& proc : cstr.processes)
        {
            on_previousProcessAdded(proc);
        }
    }
    if(m_nextConstraint)
    {
        const auto& cstr = parentScenario()->constraint(m_nextConstraint);
        for(const auto& proc : cstr.processes)
        {
            on_nextProcessAdded(proc);
        }
    }
}

const ScenarioInterface* StateModel::parentScenario() const
{
    return dynamic_cast<ScenarioInterface*>(parent());
}

double StateModel::heightPercentage() const
{
    return m_heightPercentage;
}

void StateModel::setHeightPercentage(double y)
{
    if(m_heightPercentage == y)
        return;
    m_heightPercentage = y;
    emit heightPercentageChanged();
}

void StateModel::statesUpdated_slt()
{
    emit sig_statesUpdated();
}

void StateModel::on_previousProcessAdded(const Process&)
{

}

void StateModel::on_previousProcessRemoved(const Process&)
{

}

void StateModel::on_nextProcessAdded(const Process& proc)
{
    ProcessStateDataInterface* state = proc.startState();
    connect(state, &ProcessStateDataInterface::messagesChanged,
            this, [ ] (const iscore::MessageList& ml) {
        // We merge the messages with the state.
        // No need to undo-redo this, this is an invariant.
        // TODO have some collapsing between all the processes of a state
        // NOTE how to prevent these states from being played
        // twice ? mark them ? which one shoul be sent ? the ones
        // from the process ?
//        messages() = // TODO
    });
}

void StateModel::on_nextProcessRemoved(const Process&)
{

}

const Id<EventModel> &StateModel::eventId() const
{
    return m_eventId;
}

void StateModel::setEventId(const Id<EventModel> & id)
{
    m_eventId = id;
}

const Id<ConstraintModel> &StateModel::previousConstraint() const
{
    return m_previousConstraint;
}

const Id<ConstraintModel> &StateModel::nextConstraint() const
{
    return m_nextConstraint;
}

void StateModel::setNextConstraint(const Id<ConstraintModel> & id)
{
    m_nextConstraint = id;

    if(!m_nextConstraint)
        return;

    auto& cstr = parentScenario()->constraint(m_nextConstraint);
    con(cstr.processes, &NotifyingMap<Process>::added,
        this, &StateModel::on_nextProcessAdded);
    con(cstr.processes, &NotifyingMap<Process>::removed,
        this, &StateModel::on_nextProcessRemoved);
}

void StateModel::setPreviousConstraint(const Id<ConstraintModel> & id)
{
    m_previousConstraint = id;

    if(!m_nextConstraint)
        return;

    auto& cstr = parentScenario()->constraint(m_nextConstraint);
    con(cstr.processes, &NotifyingMap<Process>::added,
        this, &StateModel::on_previousProcessAdded);
    con(cstr.processes, &NotifyingMap<Process>::removed,
        this, &StateModel::on_previousProcessRemoved);
}


const iscore::MessageItemModel& StateModel::messages() const
{
    return *m_messageItemModel;
}

iscore::MessageItemModel& StateModel::messages()
{
    return *m_messageItemModel;
}


void StateModel::setStatus(EventStatus status)
{
    if (m_status == status)
        return;

    m_status = status;
    emit statusChanged(status);
}
