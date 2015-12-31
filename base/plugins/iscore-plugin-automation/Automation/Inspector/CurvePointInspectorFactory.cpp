#include <QString>

#include "CurvePointInspectorFactory.hpp"
#include "CurvePointInspectorWidget.hpp"
#include <Curve/Point/CurvePointModel.hpp>
class InspectorWidgetBase;
class QObject;
class QWidget;
namespace iscore {
class Document;
}  // namespace iscore

InspectorWidgetBase* CurvePointInspectorFactory::makeWidget(
        const QObject& sourceElement,
        const iscore::DocumentContext& doc,
        QWidget* parent) const
{
    return new CurvePointInspectorWidget{
        safe_cast<const Curve::PointModel&>(sourceElement),
                doc,
                parent};
}

const QList<QString>&CurvePointInspectorFactory::key_impl() const
{
    static const QList<QString>& lst{"CurvePointModel"};
    return lst;
}
