#include <Process/Style/ProcessFonts.hpp>
#include <QFont>
#include <qnamespace.h>
#include <algorithm>
#include <QBrush>

#include "ClickableLabelItem.hpp"
#include <iscore/model/ModelMetadata.hpp>
#include <iscore/model/Skin.hpp>

class QGraphicsSceneHoverEvent;
class QGraphicsSceneMouseEvent;

namespace Scenario
{

SeparatorItem::SeparatorItem(QGraphicsItem *parent):
    QGraphicsSimpleTextItem{"/", parent}
{
    auto font = iscore::Skin::instance().SansFont;
    font.setPointSize(10);
    font.setBold(true);
    this->setFont(font);
    this->setBrush(Qt::white);
}


ClickableLabelItem::ClickableLabelItem(
        iscore::ModelMetadata& metadata,
        ClickHandler&& onClick,
        const QString &text,
        QGraphicsItem *parent):
    QGraphicsSimpleTextItem{text, parent},
    m_onClick{std::move(onClick)}
{
    connect(&metadata, &iscore::ModelMetadata::NameChanged,
            this, [&] (const QString& name) {
        setText(name);
        emit textChanged();
    });

    auto font = iscore::Skin::instance().SansFont;
    font.setPointSize(10);
    font.setBold(true);
    this->setFont(font);
    this->setBrush(Qt::white);

    this->setAcceptHoverEvents(true);
}


void ClickableLabelItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_onClick(this);
}


void ClickableLabelItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    this->setBrush(Qt::blue);
    update();
}

void ClickableLabelItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    this->setBrush(Qt::white);
    update();
}

int ClickableLabelItem::index() const
{
    return m_index;
}

void ClickableLabelItem::setIndex(int index)
{
    m_index = index;
}

}
