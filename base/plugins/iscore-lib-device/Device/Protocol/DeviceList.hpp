#pragma once
#include <Device/Protocol/DeviceInterface.hpp>
#include <QString>
#include <algorithm>
#include <vector>

#include <Device/Protocol/DeviceSettings.hpp>

namespace Device
{
class ISCORE_LIB_DEVICE_EXPORT DeviceList
{
    public:
        DeviceList() = default;

        DeviceList(DeviceList&&) = delete;
        DeviceList(const DeviceList&) = delete;
        DeviceList& operator=(const DeviceList&) = delete;
        DeviceList& operator=(DeviceList&&) = delete;

        bool hasDevice(const QString& name) const;

        auto find(const QString &name) const
        {
            return std::find_if(m_devices.cbegin(),
                                m_devices.cend(),
                                [&] (DeviceInterface* d) { return d->settings().name == name; });
        }


        DeviceInterface& device(const QString& name) const;

        void addDevice(DeviceInterface* dev);
        void removeDevice(const QString& name);

        const std::vector<DeviceInterface*>& devices() const;

    private:
        std::vector<DeviceInterface*> m_devices;
};
}
