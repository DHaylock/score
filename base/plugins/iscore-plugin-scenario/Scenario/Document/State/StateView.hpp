#pragma once
#include <QColor>
#include <QtGlobal>
#include <QGraphicsItem>
#include <QRect>

#include <Scenario/Document/Event/ExecutionStatus.hpp>
#include <iscore_plugin_scenario_export.h>
class QGraphicsSceneDragDropEvent;
class QGraphicsSceneMouseEvent;
class QMimeData;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

namespace Scenario
{
class StatePresenter;

class ISCORE_PLUGIN_SCENARIO_EXPORT  StateView final : public QGraphicsObject
{
        Q_OBJECT
    public:
        StateView(StatePresenter &presenter, QGraphicsItem *parent = 0);
        virtual ~StateView() = default;

        static constexpr int static_type()
        { return QGraphicsItem::UserType + 4; }
        int type() const override
        { return static_type(); }

        const StatePresenter& presenter() const
        { return m_presenter; }

        QRectF boundingRect() const override
        { return {-m_radiusFull, -m_radiusFull, 2*m_radiusFull, 2*m_radiusFull }; }

        void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

        void setContainMessage(bool);
        void setSelected(bool arg);

        void changeColor(const QColor&);
        void setStatus(ExecutionStatus);

    signals:
        void dropReceived(const QMimeData*);

    protected:
        void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
        void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
        void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
        void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
        void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
        void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
        void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;

        void dropEvent(QGraphicsSceneDragDropEvent *event) override;
    private:
        StatePresenter& m_presenter;

        bool m_containMessage{false};
        bool m_selected{false};

        QColor m_baseColor;

        ExecutionStatusProperty m_status{};

        static const constexpr qreal m_radiusFull = 7.;
        static const constexpr qreal m_radiusPoint = 3.5;
        qreal m_dilatationFactor = 1;
};
}
