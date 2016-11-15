#include <QtGlobal>

#include "RackView.hpp"
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentViewConstants.hpp>
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;


namespace Scenario
{
RackView::RackView(QGraphicsItem* parent) :
    QGraphicsItem{parent}
{
    this->setFlag(QGraphicsItem::ItemHasNoContents);
    this->setZValue(ZPos::Rack);
}

QRectF RackView::boundingRect() const
{
    return {0,
            0,
            m_width,
            m_height
           };
}

void RackView::paint(QPainter* ,
                    const QStyleOptionGraphicsItem* ,
                    QWidget* )
{
}
}
