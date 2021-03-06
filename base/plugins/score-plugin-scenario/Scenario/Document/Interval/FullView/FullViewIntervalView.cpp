// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <Process/Style/ScenarioStyle.hpp>
#include <QColor>
#include <QCursor>
#include <QGraphicsItem>
#include <QPainter>
#include <QPen>
#include <QtGlobal>
#include <qnamespace.h>

#include "FullViewIntervalPresenter.hpp"
#include "FullViewIntervalView.hpp"
#include <Scenario/Document/Interval/IntervalView.hpp>
#include <QGraphicsScene>
#include <QGraphicsView>
class QStyleOptionGraphicsItem;
class QWidget;

namespace Scenario
{
FullViewIntervalView::FullViewIntervalView(
    FullViewIntervalPresenter& presenter, QGraphicsItem* parent)
    : IntervalView{presenter, parent}
{
  this->setCacheMode(QGraphicsItem::NoCache);
  this->setParentItem(parent);
  this->setFlag(ItemIsSelectable);

  this->setZValue(1);
}

void FullViewIntervalView::updatePaths()
{
}
void FullViewIntervalView::updatePlayPaths()
{
}

void FullViewIntervalView::updateOverlayPos()
{
}

void FullViewIntervalView::setSelected(bool selected)
{
  m_selected = selected;
  setZValue(m_selected ? ZPos::SelectedInterval : ZPos::Interval);
  update();
}
QRectF FullViewIntervalView::boundingRect() const
{
  return {0, 0, qreal(std::max(defaultWidth(), m_guiWidth)) + 3, qreal(intervalAndRackHeight()) + 3};
}

void FullViewIntervalView::paint(
    QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  auto& skin = ScenarioStyle::instance();
  const qreal min_w = minWidth();
  const qreal max_w = maxWidth();
  const qreal def_w = defaultWidth();
  const qreal play_w = playWidth();

  auto& p = *painter;

  p.setRenderHint(QPainter::Antialiasing, false);

  QBrush c;
  if (isSelected())
  {
    c = skin.IntervalSelected.getBrush();
  }
  else if (parentItem()->isSelected())
  {
    c = skin.IntervalFullViewParentSelected.getBrush();
  }
  else
  {
    c = skin.IntervalBase.getBrush();
  }

  skin.IntervalSolidPen.setBrush(c);

  if (min_w == max_w)
  {
    p.setPen(skin.IntervalSolidPen);
    p.drawLine(QPointF{0., 0.}, QPointF{def_w, 0.});
  }
  else
  {
    // Then the dashed line
    skin.IntervalDashPen.setBrush(c);
    p.setPen(skin.IntervalDashPen);

    p.fillRect(QRectF{def_w, (double)IntervalHeaderHeight, m_guiWidth, this->height() - IntervalHeaderHeight}, skin.SlotOverlay.getBrush());

    p.drawLine(QPointF{min_w, 0.}, QPointF{infinite()? m_guiWidth : max_w, 0.});


    painter->setPen(skin.FullViewIntervalHeaderSeparator);
    p.drawLine(QPointF{def_w, (double)IntervalHeaderHeight}, QPointF{m_guiWidth, (double)IntervalHeaderHeight});

    // First the line going from 0 to the min
    p.setPen(skin.IntervalSolidPen);
    p.drawLine(QPointF{0., 0.}, QPointF{min_w, 0.});

  }

  auto pw = playWidth();
  if (pw != 0.)
  {
    skin.IntervalPlayPen.setBrush(skin.IntervalPlayFill.getBrush());
    p.setPen(skin.IntervalPlayPen);
    p.drawLine(QPointF{0., 0.}, QPointF{std::min(play_w, std::max(def_w, max_w)), 0.});
  }

#if defined(SCORE_SCENARIO_DEBUG_RECTS)
  p.setPen(Qt::red);
  p.drawRect(boundingRect());
#endif
}

void FullViewIntervalView::setGuiWidth(double w)
{
  m_guiWidth = w;
  update();
}
}
