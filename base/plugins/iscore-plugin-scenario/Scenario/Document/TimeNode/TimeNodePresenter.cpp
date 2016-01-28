#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>
#include <Scenario/Document/TimeNode/TimeNodeView.hpp>
#include <Scenario/Document/TimeNode/Trigger/TriggerPresenter.hpp>
#include <iscore/widgets/GraphicsItem.hpp>

#include <Process/ModelMetadata.hpp>
#include "TimeNodePresenter.hpp"
#include <iscore/selection/Selectable.hpp>
#include <iscore/tools/NamedObject.hpp>
#include <iscore/tools/Todo.hpp>

class QObject;
#include <iscore/tools/SettableIdentifier.hpp>

namespace Scenario
{
TimeNodePresenter::TimeNodePresenter(const TimeNodeModel& model,
                                     QGraphicsObject *parentview,
                                     QObject* parent) :
    NamedObject {"TimeNodePresenter", parent},
    m_model {model},
    m_view {new TimeNodeView{*this, parentview}}
{
    m_triggerPres = new TriggerPresenter{*m_model.trigger(), m_view, this };

    con(m_model.selection, &Selectable::changed,
            m_view, &TimeNodeView::setSelected);

    con(m_model, &TimeNodeModel::newEvent,
            this,     &TimeNodePresenter::on_eventAdded);

    con((m_model.metadata), &ModelMetadata::colorChanged,
            m_view,               &TimeNodeView::changeColor);

    // TODO find a correct way to handle validity of model elements.
    // extentChanged is updated in scenario.
}

TimeNodePresenter::~TimeNodePresenter()
{
    deleteGraphicsObject(m_view);
}

const Id<TimeNodeModel>& TimeNodePresenter::id() const
{
    return m_model.id();
}

const TimeNodeModel& TimeNodePresenter::model() const
{
    return m_model;
}

TimeNodeView* TimeNodePresenter::view() const
{
    return m_view;
}

void TimeNodePresenter::on_eventAdded(const Id<EventModel>& eventId)
{
    emit eventAdded(eventId, m_model.id());
}
}

