#include <QVBoxLayout>
#include <QLabel>

#include <Device/Protocol/ProtocolSettingsWidget.hpp>
#include "LocalProtocolSettingsWidget.hpp"
#include "LocalSpecificSettings.hpp"

class QWidget;

namespace Engine
{
namespace Network
{
LocalProtocolSettingsWidget::LocalProtocolSettingsWidget(QWidget* parent)
    : ProtocolSettingsWidget(parent)
{
    auto lay = new QVBoxLayout;
    QLabel* deviceNameLabel = new QLabel(tr("Local device"), this);
    lay->addWidget(deviceNameLabel);
    setLayout(lay);
}

Device::DeviceSettings LocalProtocolSettingsWidget::getSettings() const
{
    Device::DeviceSettings s;
    // TODO *** protocol is never set here. Check everywhere.! ***
    s.name = "i-score";
    Network::LocalSpecificSettings local;
    s.deviceSpecificSettings = QVariant::fromValue(local);
    return s;
}

void
LocalProtocolSettingsWidget::setSettings(const Device::DeviceSettings &settings)
{
    if(settings.deviceSpecificSettings.canConvert<Network::LocalSpecificSettings>())
    {
    }
}
}
}
