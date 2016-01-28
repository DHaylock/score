#pragma once

#include <Scenario/Document/TimeRuler/AbstractTimeRulerView.hpp>
#include <QRect>

namespace Scenario
{
class LocalTimeRulerView final : public AbstractTimeRulerView
{
    public:
        LocalTimeRulerView();
        ~LocalTimeRulerView();

        QRectF boundingRect() const override;

};
}
