#pragma once

#include <Process/State/ProcessStateDataInterface.hpp>
#include <State/Message.hpp>
#include <QString>
#include <vector>

#include <Process/State/MessageNode.hpp>

class QObject;
namespace iscore {
struct Address;
}  // namespace iscore

namespace Automation
{
class ProcessModel;
class State final : public ProcessStateDataInterface
{
    public:
        // watchedPoint : something between 0 and 1
        State(
                ProcessModel& process,
                double watchedPoint,
                QObject* parent);

        QString stateName() const override;
        ProcessModel& process() const;

        iscore::Message message() const;
        double point() const;

        State* clone(QObject* parent) const override;

        std::vector<iscore::Address> matchingAddresses() override;
        iscore::MessageList messages() const override;
        iscore::MessageList setMessages(const iscore::MessageList&, const MessageNode&) override;

    private:
        double m_point{};
};

}
