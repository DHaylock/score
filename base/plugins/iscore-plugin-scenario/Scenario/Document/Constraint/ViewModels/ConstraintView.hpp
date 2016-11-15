#pragma once
#include <QBrush>
#include <QGraphicsItem>
#include <qnamespace.h>
#include <QPen>
#include <iscore_plugin_scenario_export.h>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentViewConstants.hpp>
class QGraphicsSceneMouseEvent;

namespace Scenario
{
class ConstraintPresenter;
class LeftBraceView;
class RightBraceView;
class ISCORE_PLUGIN_SCENARIO_EXPORT ConstraintView :
        public QObject,
        public QGraphicsItem
{
        Q_OBJECT
        Q_INTERFACES(QGraphicsItem)

    public:
        ConstraintView(ConstraintPresenter& presenter,
                       QGraphicsItem* parent);
        virtual ~ConstraintView();

        static constexpr int static_type()
        { return QGraphicsItem::UserType + ItemType::Constraint; }
        int type() const final override
        { return static_type(); }

        const ConstraintPresenter& presenter() const
        { return m_presenter;}


        void setInfinite(bool);
        bool infinite() const
        { return m_infinite; }

        void setDefaultWidth(double width);
        void setMaxWidth(bool infinite, double max);
        void setMinWidth(double min);
        void setHeight(double height);
        void setPlayWidth(double width);
        void setValid(bool val);

        double height() const
        { return m_height; }

        void setSelected(bool selected)
        {
            m_selected = selected;
            update();
        }

        bool isSelected() const
        {
            return m_selected;
        }


        double defaultWidth() const
        {
            return m_defaultWidth;
        }

        double minWidth() const
        {
            return m_minWidth;
        }

        double maxWidth() const
        {
            return m_maxWidth;
        }

        double constraintAndRackHeight() const
        {
            return m_height;
        }

        double playWidth() const
        {
            return m_playWidth;
        }

        bool isValid() const
        {
            return m_validConstraint;
        }

        bool warning() const;
        void setWarning(bool warning);

        void mousePressEvent(QGraphicsSceneMouseEvent *event) final override;
        void mouseMoveEvent(QGraphicsSceneMouseEvent *event) final override;
        void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) final override;

        LeftBraceView& leftBrace() const
        { return *m_leftBrace; }
        RightBraceView& rightBrace() const
        { return *m_rightBrace; }
    protected:

    private:
        ConstraintPresenter& m_presenter;
        LeftBraceView* m_leftBrace{};
        RightBraceView* m_rightBrace{};
        double m_defaultWidth {};
        double m_maxWidth {};
        double m_minWidth {};
        double m_playWidth {};

        double m_height {};

        bool m_selected{};
        bool m_infinite{};
        bool m_validConstraint{true};
        bool m_warning{false};
};
}
