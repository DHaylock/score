#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>

#include <QtAlgorithms>
#include <QMap>
#include <QObject>
#include <QString>
#include <algorithm>
#include <cstddef>
#include <vector>

#include "AddressBarItem.hpp"
#include "ClickableLabelItem.hpp"
#include <Process/ModelMetadata.hpp>
#include <iscore/tools/ObjectIdentifier.hpp>

class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

AddressBarItem::AddressBarItem(QGraphicsItem *parent):
    QGraphicsObject{parent}
{

}

void AddressBarItem::setTargetObject(ObjectPath && path)
{
    qDeleteAll(m_items);
    m_items.clear();

    m_currentPath = std::move(path);

    double currentWidth = 0;
    int i = -1;

    auto& constraint = m_currentPath.find<ConstraintModel>();
    auto cstr = &constraint;
    QMap<int, QString> name;

    for(int j = m_currentPath.vec().size() - 1 ; j > -1 ; j--)
    {
        if (! m_currentPath.vec().at(j).objectName().contains("ConstraintModel") )
          continue;

        name[j] = (cstr->metadata.name());

        auto scenar = dynamic_cast<Scenario::ScenarioModel*>(cstr->parent());
        if(scenar)
            cstr = safe_cast<ConstraintModel*>(scenar->parent());
    }


    for(auto& identifier : m_currentPath)
    {
        i++;
        if(!identifier.objectName().contains("ConstraintModel"))
            continue;

        auto thisPath = m_currentPath;
        auto& pathVec = thisPath.vec();
        pathVec.resize(i + 1);
        ConstraintModel& thisObj = thisPath.find<ConstraintModel>();

        QString txt = name.value(i);

        auto lab = new ClickableLabelItem{
                [&] (ClickableLabelItem* item) {
                    emit constraintSelected(thisObj);
                },
                txt,
                this};

        lab->setIndex(i);

        m_items.append(lab);
        lab->setPos(currentWidth, 0);
        currentWidth += 10 + lab->boundingRect().width();

        auto sep = new SeparatorItem{this};
        sep->setPos(currentWidth, 0);
        currentWidth += 10 + sep->boundingRect().width();
        m_items.append(sep);
    }

    prepareGeometryChange();
    m_width = currentWidth;
}


double AddressBarItem::width() const
{
    return m_width;
}



QRectF AddressBarItem::boundingRect() const
{
    return {};
}

void AddressBarItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
}
