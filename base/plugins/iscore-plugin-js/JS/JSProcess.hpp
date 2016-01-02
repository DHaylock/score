#pragma once
#include <OSSIA/ProcessModel/TimeProcessWithConstraint.hpp>
#include <QJSEngine>
#include <QJSValue>
#include <QString>
#include <memory>

#include "Editor/TimeValue.h"

class DeviceDocumentPlugin;
class DeviceList;
namespace OSSIA {
class State;
class StateElement;
}  // namespace OSSIA

namespace JS
{
class ProcessExecutor final :
        public TimeProcessWithConstraint
{
    public:
        ProcessExecutor(DeviceDocumentPlugin& devices);

        void setTickFun(const QString& val);

        std::shared_ptr<OSSIA::StateElement> state(
                const OSSIA::TimeValue&,
                const OSSIA::TimeValue&) override;

        const std::shared_ptr<OSSIA::State>& getStartState() const override
        {
            return m_start;
        }

        const std::shared_ptr<OSSIA::State>& getEndState() const override
        {
            return m_end;
        }


    private:
        DeviceList& m_devices;
        QJSEngine m_engine;
        QJSValue m_tickFun;

        std::shared_ptr<OSSIA::State> m_start;
        std::shared_ptr<OSSIA::State> m_end;
};
}
