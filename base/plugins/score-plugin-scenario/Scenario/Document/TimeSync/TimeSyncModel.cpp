// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <Process/Style/ScenarioStyle.hpp>
#include <QtGlobal>
#include <Scenario/Document/Event/EventModel.hpp>

#include "TimeSyncModel.hpp"
#include <Process/TimeValue.hpp>
#include <Scenario/Document/VerticalExtent.hpp>
#include <Scenario/Process/ScenarioInterface.hpp>
#include <score/document/DocumentInterface.hpp>
#include <score/model/ModelMetadata.hpp>
#include <score/model/IdentifiedObject.hpp>
#include <score/model/Identifier.hpp>

namespace Scenario
{
TimeSyncModel::TimeSyncModel(
    const Id<TimeSyncModel>& id,
    const VerticalExtent& extent,
    const TimeVal& date,
    QObject* parent)
    : Entity{id, Metadata<ObjectKey_k, TimeSyncModel>::get(), parent}
    , m_extent{extent}
    , m_date{date}
{
  metadata().setInstanceName(*this);
  metadata().setColor(ScenarioStyle::instance().TimenodeDefault);
}

TimeSyncModel::TimeSyncModel(
    const TimeSyncModel& source, const Id<TimeSyncModel>& id, QObject* parent)
    : Entity{source, id, Metadata<ObjectKey_k, TimeSyncModel>::get(), parent}
    , m_extent{source.m_extent}
    , m_date{source.m_date}
    , m_expression{source.expression()}
    , m_active{source.active()}
    , m_events(source.m_events)
{

}

void TimeSyncModel::addEvent(const Id<EventModel>& eventId)
{
  m_events.push_back(eventId);

  auto scenar = dynamic_cast<ScenarioInterface*>(parent());
  if (scenar)
  {
    // There may be no scenario when we are cloning without a parent.
    // TODO this addEvent should be in an outside algorithm.
    auto& theEvent = scenar->event(eventId);
    theEvent.changeTimeSync(this->id());
  }

  emit newEvent(eventId);
}

bool TimeSyncModel::removeEvent(const Id<EventModel>& eventId)
{
  if (m_events.indexOf(eventId) >= 0)
  {
    m_events.remove(m_events.indexOf(eventId));
    emit eventRemoved(eventId);
    return true;
  }

  return false;
}

void TimeSyncModel::clearEvents()
{
  auto ev = m_events;
  m_events.clear();
  for(const auto& e : ev)
    emit eventRemoved(e);
}

const TimeVal& TimeSyncModel::date() const
{
  return m_date;
}

void TimeSyncModel::setDate(const TimeVal& date)
{
  m_date = date;
  emit dateChanged(m_date);
}

const QVector<Id<EventModel>>& TimeSyncModel::events() const
{
  return m_events;
}

void TimeSyncModel::setEvents(const QVector<Id<EventModel>>& events)
{
  m_events = events;
}

const VerticalExtent& TimeSyncModel::extent() const
{
  return m_extent;
}

void TimeSyncModel::setExtent(const VerticalExtent& extent)
{
  if (extent != m_extent)
  {
    m_extent = extent;
    emit extentChanged(m_extent);
  }
}

void TimeSyncModel::setExpression(const State::Expression& expression)
{
  if (m_expression == expression)
    return;
  m_expression = expression;
  emit triggerChanged(m_expression);
}

bool TimeSyncModel::active() const
{
  return m_active;
}

void TimeSyncModel::setActive(bool active)
{
  if (active == m_active)
    return;
  m_active = active;
  emit activeChanged();
}
}
