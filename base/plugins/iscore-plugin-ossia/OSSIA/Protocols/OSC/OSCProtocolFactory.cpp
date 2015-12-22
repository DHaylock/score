#include <QObject>


#include <Device/Protocol/DeviceSettings.hpp>
#include "OSCDevice.hpp"
#include "OSCProtocolFactory.hpp"
#include "OSCProtocolSettingsWidget.hpp"
#include <OSSIA/Protocols/OSC/OSCSpecificSettings.hpp>

class DeviceInterface;
class ProtocolSettingsWidget;
struct VisitorVariant;

QString OSCProtocolFactory::prettyName() const
{
    return QObject::tr("OSC");
}

const ProtocolFactoryKey&OSCProtocolFactory::key_impl() const
{
    static const ProtocolFactoryKey name{"OSC"};
    return name;
}

DeviceInterface*OSCProtocolFactory::makeDevice(
        const iscore::DeviceSettings& settings,
        const iscore::DocumentContext& ctx)
{
    return new OSCDevice{settings};
}

const iscore::DeviceSettings&OSCProtocolFactory::defaultSettings() const
{
    static const iscore::DeviceSettings settings = [&] () {
        iscore::DeviceSettings s;
        s.protocol = key_impl();
        s.name = "OSC";
        OSCSpecificSettings specif;
        s.deviceSpecificSettings = QVariant::fromValue(specif);
        return s;
    }();
    return settings;
}

ProtocolSettingsWidget*OSCProtocolFactory::makeSettingsWidget()
{
    return new OSCProtocolSettingsWidget;
}

QVariant OSCProtocolFactory::makeProtocolSpecificSettings(const VisitorVariant& visitor) const
{
    return makeProtocolSpecificSettings_T<OSCSpecificSettings>(visitor);
}

void OSCProtocolFactory::serializeProtocolSpecificSettings(const QVariant& data, const VisitorVariant& visitor) const
{
    serializeProtocolSpecificSettings_T<OSCSpecificSettings>(data, visitor);
}

bool OSCProtocolFactory::checkCompatibility(const iscore::DeviceSettings& a, const iscore::DeviceSettings& b) const
{
    auto a_p = a.deviceSpecificSettings.value<OSCSpecificSettings>();
    auto b_p = b.deviceSpecificSettings.value<OSCSpecificSettings>();
    return a.name != b.name && a_p.inputPort != b_p.inputPort && a_p.outputPort != b_p.outputPort;
}
