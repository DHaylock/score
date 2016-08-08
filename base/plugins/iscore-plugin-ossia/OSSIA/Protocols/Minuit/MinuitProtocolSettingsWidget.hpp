#pragma once

#include <Device/Protocol/ProtocolSettingsWidget.hpp>

#include <Device/Protocol/DeviceSettings.hpp>

class QLineEdit;
class QSpinBox;
class QWidget;

namespace Engine
{
namespace Network
{
class MinuitProtocolSettingsWidget :
        public Device::ProtocolSettingsWidget
{
    public:
        MinuitProtocolSettingsWidget(QWidget* parent = nullptr);

        Device::DeviceSettings getSettings() const override;

        void setSettings(const Device::DeviceSettings& settings) override;

    protected:
        void buildGUI();

        void setDefaults();

    protected:
        QLineEdit* m_deviceNameEdit;
        QSpinBox* m_portInputSBox;
        QSpinBox* m_portOutputSBox;
        QLineEdit* m_localHostEdit;

};
}
}
