#pragma once
#include <Process/Inspector/ProcessInspectorWidgetDelegateFactory.hpp>
#include <RecordedMessages/RecordedMessagesProcessMetadata.hpp>
#include <QList>
#include <QString>
namespace RecordedMessages
{
class ProcessModel;
}
class InspectorWidgetBase;
class QObject;
class QWidget;
namespace iscore {
class Document;
}  // namespace iscore

namespace RecordedMessages
{

class InspectorFactory final :
        public Process::InspectorWidgetDelegateFactory
{
        ISCORE_CONCRETE_FACTORY("cc0c927c-947e-4aed-b2b9-9eab2903c63d")
    public:
        InspectorFactory();
        virtual ~InspectorFactory();

    private:
        Process::InspectorWidgetDelegate* make(
                const Process::ProcessModel&,
                const iscore::DocumentContext&,
                QWidget* parent) const override;
        bool matches(const Process::ProcessModel&) const override;
};

}
