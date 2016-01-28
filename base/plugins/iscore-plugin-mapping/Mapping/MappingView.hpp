#pragma once

#include <Process/LayerView.hpp>
#include <QString>

class QGraphicsItem;
class QPainter;

namespace Mapping
{
class MappingView final : public Process::LayerView
{
    public:
        explicit MappingView(QGraphicsItem *parent);

        void setSourceName(const QString& s) { m_source = s;}
        void setDestinationName(const QString& s) { m_dest = s;}
        void showName(bool b)  { m_showName = b; }

    protected:
        void paint_impl(QPainter* painter) const override;

    private:
        QString m_source;
        QString m_dest;
        bool m_showName{true};
};
}
