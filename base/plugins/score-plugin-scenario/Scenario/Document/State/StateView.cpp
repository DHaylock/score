// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <Process/Style/ScenarioStyle.hpp>
#include <QBrush>
#include <QCursor>
#include <QGraphicsSceneEvent>
#include <QPainter>
#include <QPen>
#include <qnamespace.h>

#include "StateMenuOverlay.hpp"
#include "StatePresenter.hpp"
#include "StateView.hpp"

class QStyleOptionGraphicsItem;
class QWidget;
namespace Scenario
{
static const QPainterPath smallNonDilated{
[] {
    QPainterPath p;
    p.addEllipse({0, 0}, StateView::pointRadius, StateView::pointRadius);
    return p;
  }()
};
static const QPainterPath fullNonDilated{
  [] {
    QPainterPath p;
    p.addEllipse({0, 0}, StateView::fullRadius, StateView::fullRadius);
    return p;
  }()
};
static const QPainterPath smallDilated{
[] {
    QPainterPath p;
    p.addEllipse({0, 0},
                 StateView::pointRadius * StateView::dilated,
                 StateView::pointRadius * StateView::dilated);
    return p;
  }()
};
static const QPainterPath fullDilated{
  [] {
    QPainterPath p;
    p.addEllipse({0, 0},
                 StateView::fullRadius * StateView::dilated,
                 StateView::fullRadius * StateView::dilated);
    return p;
  }()
};
StateView::StateView(StatePresenter& pres, QGraphicsItem* parent)
    : QGraphicsItem(parent), m_presenter{pres}
{
  this->setCacheMode(QGraphicsItem::NoCache);
  this->setParentItem(parent);

  this->setCursor(QCursor(Qt::SizeAllCursor));
  this->setZValue(ZPos::State);
  this->setAcceptDrops(true);
  this->setAcceptHoverEvents(true);
}

void StateView::paint(
    QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  painter->setRenderHint(QPainter::Antialiasing, true);
  auto& skin = ScenarioStyle::instance();
  painter->setPen(skin.NoPen);
  skin.StateTemporalPointBrush =
      m_selected ? skin.StateSelected.getBrush() : skin.StateDot.getBrush();

  auto status = m_status.get();
  if (status != ExecutionStatus::Editing)
    skin.StateTemporalPointBrush = m_status.stateStatusColor().getBrush();

  if (m_containMessage)
  {
    painter->setBrush(skin.StateOutline.getBrush());
    if(m_dilated)
      painter->drawPath(fullDilated);
    else
      painter->drawPath(fullNonDilated);
  }

  skin.StateTemporalPointPen.setBrush(skin.StateTemporalPointBrush);

  painter->setBrush(skin.StateTemporalPointBrush);
  painter->setPen(skin.StateTemporalPointPen);
  if(m_dilated)
    painter->drawPath(smallDilated);
  else
    painter->drawPath(smallNonDilated);

#if defined(SCORE_SCENARIO_DEBUG_RECTS)
  painter->setBrush(Qt::NoBrush);
  painter->setPen(Qt::darkYellow);
  painter->drawRect(boundingRect());
#endif

  painter->setRenderHint(QPainter::Antialiasing, false);
}

void StateView::setContainMessage(bool arg)
{
  m_containMessage = arg;
  update();
}

void StateView::setSelected(bool b)
{
  m_selected = b;
  setDilatation(m_selected);

  if(m_hasOverlay)
  {
      if(b)
      {
        m_overlay = new StateMenuOverlay{this};
        m_overlay->setPos(14, -14);
      }
      else
      {
        delete m_overlay;
        m_overlay = nullptr;
      }
  }
}

void StateView::setStatus(ExecutionStatus status)
{
  if (m_status.get() == status)
    return;
  m_status.set(status);
  update();
}

void StateView::disableOverlay()
{
    m_hasOverlay = false;
}

void StateView::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if (event->button() == Qt::MouseButton::LeftButton)
    emit m_presenter.pressed(event->scenePos());
}

void StateView::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  emit m_presenter.moved(event->scenePos());
}

void StateView::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  emit m_presenter.released(event->scenePos());
}

void StateView::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
}

void StateView::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
}
void StateView::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
  setDilatation(true);
}

void StateView::dragLeaveEvent(QGraphicsSceneDragDropEvent* event)
{
  setDilatation(m_selected);
}

void StateView::dropEvent(QGraphicsSceneDragDropEvent* event)
{
  emit dropReceived(event->mimeData());
}

void StateView::setDilatation(bool val)
{
  prepareGeometryChange();
  m_dilated = val;
  this->update();
}
}
