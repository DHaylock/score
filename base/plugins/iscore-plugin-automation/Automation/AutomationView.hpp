#pragma once

#include <Process/LayerView.hpp>
#include <QString>

class QGraphicsItem;
class QPainter;

class AutomationView final : public Process::LayerView
{
    public:
        explicit AutomationView(QGraphicsItem *parent);

        void setDisplayedName(QString s) {m_displayedName = s;}
        void showName(bool b) {m_showName = b;}

    protected:
        void paint_impl(QPainter* painter) const override;

    private:
        QString m_displayedName{};
        bool m_showName{true};
};
