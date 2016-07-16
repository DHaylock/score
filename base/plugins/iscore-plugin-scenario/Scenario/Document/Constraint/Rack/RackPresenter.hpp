#pragma once
#include <Process/TimeValue.hpp>
#include <Process/ZoomHelper.hpp>
#include <iscore/tools/IdentifiedObjectMap.hpp>
#include <iscore/tools/NamedObject.hpp>
#include <iscore/widgets/GraphicsItem.hpp>
#include <QtGlobal>
#include <QPoint>

#include <nano_signal_slot.hpp>
#include "Slot/SlotModel.hpp"
#include "Slot/SlotPresenter.hpp"

class QObject;
#include <iscore/tools/SettableIdentifier.hpp>

namespace Scenario
{
class RackModel;
class RackView;
class RackPresenter final :
        public QObject,
        public Nano::Observer
{
        Q_OBJECT

    public:
        RackPresenter(const RackModel& model,
                     RackView* view,
                     const Process::ProcessPresenterContext&,
                     QObject* parent);
        virtual ~RackPresenter();

        const RackModel& model() const
        { return m_model; }
        const RackView& view() const;

        qreal height() const;
        qreal width() const;
        void setWidth(qreal);

        const Id<RackModel>& id() const;
        const IdContainer<SlotPresenter,SlotModel>& getSlots() const // here we use the 'get' prefix, because 'slots' is keyWord for Qt ...
        { return m_slots; }

        void setDisabledSlotState();
        void setEnabledSlotState();

        void on_durationChanged(const TimeValue&);

        void on_askUpdate();

        void on_zoomRatioChanged(ZoomRatio);
        void on_slotPositionsChanged();

    signals:
        void askUpdate();

        void pressed(const QPointF&);
        void moved(const QPointF&);
        void released(const QPointF&);

    private:
        void on_slotCreated(const SlotModel&);
        void on_slotRemoved(const SlotModel&);

        void on_slotCreated_impl(const SlotModel& m);

        // Updates the shape of the view
        void updateShape();

        const RackModel& m_model;
        graphics_item_ptr<RackView> m_view;
        IdContainer<SlotPresenter,SlotModel> m_slots;

        const Process::ProcessPresenterContext& m_context;

        ZoomRatio m_zoomRatio{};
        TimeValue m_duration {};
};
}
