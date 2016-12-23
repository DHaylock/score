#pragma once
#include <Device/Node/DeviceNode.hpp>
#include <QString>

namespace Device
{
// Will load a device in a Jamoma-format JSON file into the node.
// The node has to be the device node.
ISCORE_LIB_DEVICE_EXPORT void
loadDeviceFromBlueYetiJSON(const QString& filePath, Device::Node& node);
}
