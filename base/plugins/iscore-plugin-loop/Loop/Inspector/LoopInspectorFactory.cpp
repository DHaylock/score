#include <Inspector/InspectorWidgetList.hpp>
#include <Loop/LoopProcessModel.hpp>
#include <Process/ProcessList.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Inspector/Constraint/ConstraintInspectorDelegateFactory.hpp>
#include <Scenario/Inspector/Constraint/ConstraintInspectorWidget.hpp>

#include <boost/optional/optional.hpp>

#include <QByteArray>
#include <QDataStream>
#include <QtGlobal>
#include <QObject>
#include <algorithm>
#include <list>

#include <Inspector/InspectorWidgetFactoryInterface.hpp>
#include "LoopInspectorFactory.hpp"
#include "LoopInspectorWidget.hpp"
#include <Process/ExpandMode.hpp>
#include <Process/TimeValue.hpp>
#include <Scenario/Commands/MoveBaseEvent.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Inspector/Constraint/ConstraintInspectorDelegate.hpp>
#include <iscore/application/ApplicationContext.hpp>
#include <iscore/document/DocumentContext.hpp>
#include <iscore/command/Dispatchers/OngoingCommandDispatcher.hpp>
#include <iscore/plugins/customfactory/StringFactoryKey.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/tools/ModelPathSerialization.hpp>

class InspectorWidgetBase;
class QWidget;

// TODO cleanup this file

class LoopConstraintInspectorDelegate final : public ConstraintInspectorDelegate
{
    public:
        LoopConstraintInspectorDelegate(const ConstraintModel& cst);

        void updateElements() override;
        void addWidgets_pre(std::list<QWidget*>& widgets, ConstraintInspectorWidget* parent) override;
        void addWidgets_post(std::list<QWidget*>& widgets, ConstraintInspectorWidget* parent) override;

        void on_defaultDurationChanged(
                OngoingCommandDispatcher& dispatcher,
                const TimeValue& val,
                ExpandMode) const override;
};


LoopConstraintInspectorDelegate::LoopConstraintInspectorDelegate(
        const ConstraintModel& cst):
    ConstraintInspectorDelegate{cst}
{
}

void LoopConstraintInspectorDelegate::updateElements()
{
}

void LoopConstraintInspectorDelegate::addWidgets_pre(
        std::list<QWidget*>& widgets,
        ConstraintInspectorWidget* parent)
{
}

void LoopConstraintInspectorDelegate::addWidgets_post(
        std::list<QWidget*>& widgets,
        ConstraintInspectorWidget* parent)
{
}

void LoopConstraintInspectorDelegate::on_defaultDurationChanged(
        OngoingCommandDispatcher& dispatcher,
        const TimeValue& val,
        ExpandMode expandmode) const
{
    auto& loop = *safe_cast<Loop::ProcessModel*>(m_model.parent());
    dispatcher.submitCommand<MoveBaseEvent<Loop::ProcessModel>>(
            loop,
            loop.state(m_model.endState()).eventId(),
            m_model.startDate() + val,
            expandmode);
}





LoopConstraintInspectorDelegateFactory::~LoopConstraintInspectorDelegateFactory()
{

}

std::unique_ptr<ConstraintInspectorDelegate> LoopConstraintInspectorDelegateFactory::make(
        const ConstraintModel& constraint)
{
    return std::make_unique<LoopConstraintInspectorDelegate>(constraint);
}

bool LoopConstraintInspectorDelegateFactory::matches(
        const ConstraintModel& constraint) const
{
    return dynamic_cast<Loop::ProcessModel*>(constraint.parent());
}



LoopInspectorFactory::LoopInspectorFactory() :
    ProcessInspectorWidgetDelegateFactory {}
{

}

LoopInspectorFactory::~LoopInspectorFactory()
{

}

ProcessInspectorWidgetDelegate* LoopInspectorFactory::make(
        const Process& process,
        const iscore::DocumentContext& doc,
        QWidget* parent) const
{
    return new LoopInspectorWidget{
        static_cast<const Loop::ProcessModel&>(process),
                doc,
                parent};
}

bool LoopInspectorFactory::matches(const Process& process) const
{
    return dynamic_cast<const Loop::ProcessModel*>(&process);
}
