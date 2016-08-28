#pragma once
#include <QtGlobal>
#include <QGraphicsItem>
#include <QPoint>
#include <QRect>
#include <QString>

#include <iscore_plugin_scenario_export.h>
class QGraphicsSceneContextMenuEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

namespace Scenario
{
class SlotHandle;
class SlotOverlay;
class SlotPresenter;

class ISCORE_PLUGIN_SCENARIO_EXPORT SlotView final :
        public QObject,
        public QGraphicsItem
{
        Q_OBJECT
  Q_INTERFACES(QGraphicsItem)

    public:
        const SlotPresenter& presenter;

        SlotView(const SlotPresenter&pres, QGraphicsItem* parent);
        virtual ~SlotView() = default;

        QRectF boundingRect() const override;
        void paint(QPainter* painter,
                           const QStyleOptionGraphicsItem* option,
                           QWidget* widget) override;

        void setHeight(qreal height);
        qreal height() const;

        void setWidth(qreal width);
        qreal width() const;

        void enable();
        void disable();

        void setFocus(bool b);

        void setFrontProcessName(const QString&);

    signals:
        void askContextMenu(const QPoint&, const QPointF&);

    private:
        void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

        qreal m_height {};
        qreal m_width {};
        SlotOverlay* m_overlay{};
        SlotHandle* m_handle{};
        bool m_focus{false};
        QString m_frontProcessName{};
};
}
