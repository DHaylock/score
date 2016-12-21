#include "ConstraintBrace.hpp"
#include <QCursor>
#include <QGraphicsSceneEvent>
#include <QPainter>
#include <QPen>

#include <Process/Style/ScenarioStyle.hpp>
#include <Scenario/Document/Constraint/ViewModels/ConstraintPresenter.hpp>

using namespace Scenario;

ConstraintBrace::ConstraintBrace(
    const ConstraintView& parentCstr, QGraphicsItem* parent)
    : QGraphicsItem(), m_parent{parentCstr}
{
  this->setCacheMode(QGraphicsItem::NoCache);
  this->setCursor(Qt::SizeHorCursor);
  this->setZValue(ZPos::Brace);

  m_path.moveTo(10, -10);
  m_path.arcTo(0, -10, 20, 20, 90, 180);

  this->setParentItem(parent);
}

QRectF ConstraintBrace::boundingRect() const
{
  return {0, -10, 10, 20};
}

void ConstraintBrace::paint(
    QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  painter->setBrush({});
  painter->setRenderHint(QPainter::Antialiasing, true);
  QPen pen{{}, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};
  // TODO make a switch instead and transform these to use Q_FLAGS or something
  if (m_parent.isSelected())
  {
    pen.setColor(ScenarioStyle::instance().ConstraintSelected.getColor());
  }
  else if (m_parent.warning())
  {
    pen.setColor(ScenarioStyle::instance().ConstraintWarning.getColor());
  }
  else
  {
    pen.setColor(ScenarioStyle::instance().ConstraintBase.getColor());
  }

  if (!m_parent.isValid())
  {
    pen.setColor(ScenarioStyle::instance().ConstraintInvalid.getColor());
  }

  painter->setPen(pen);

  painter->drawPath(m_path);

#if defined(ISCORE_SCENARIO_DEBUG_RECTS)
  painter->setPen(Qt::lightGray);
  painter->setBrush(Qt::NoBrush);
  painter->drawRect(boundingRect());
#endif
}

void ConstraintBrace::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if (event->button() == Qt::MouseButton::LeftButton)
    m_parent.presenter().pressed(event->scenePos());
}

void ConstraintBrace::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  m_parent.presenter().moved(event->scenePos());
}

void ConstraintBrace::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  m_parent.presenter().released(event->scenePos());
}
