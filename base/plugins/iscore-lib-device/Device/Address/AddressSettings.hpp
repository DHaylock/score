#pragma once
#include <QString>
#include <QVariant>
#include <QVariantList>
#include "IOType.hpp"
#include "ClipMode.hpp"
#include "Domain.hpp"
#include <State/Message.hpp>
#include <iscore/tools/Metadata.hpp>
#include <iscore_lib_device_export.h>

template<typename T>
class TreeNode;
namespace Device
{
class DeviceExplorerNode;
using Node = TreeNode<DeviceExplorerNode>;

using RefreshRate = int;
using RepetitionFilter = bool;
struct AddressSettingsCommon
{
    State::Value value;
    Device::Domain domain;

    Device::IOType ioType{};
    Device::ClipMode clipMode{};
    QString unit;

    Device::RepetitionFilter repetitionFilter{};
    Device::RefreshRate rate{};

    int priority{};

    QStringList tags;
};

// this one has only the name of the current node (e.g. 'a' for dev:/azazd/a)
struct AddressSettings : public Device::AddressSettingsCommon
{
        QString name;
};

// This one has the whole path of the node in address
struct FullAddressSettings : public Device::AddressSettingsCommon
{
        struct as_parent;
        struct as_child;
        State::Address address;

        template<typename T>
        ISCORE_LIB_DEVICE_EXPORT static FullAddressSettings make(
                const Device::AddressSettings& other,
                const State::Address& addr);

        ISCORE_LIB_DEVICE_EXPORT static FullAddressSettings make(
                const State::Message& mess);

        ISCORE_LIB_DEVICE_EXPORT static FullAddressSettings make(
                const Device::Node& node);
        // Specializations are in FullAddressSettings.cpp
};

inline bool operator==(
        const Device::AddressSettingsCommon& lhs,
        const Device::AddressSettingsCommon& rhs)
{
    return
            lhs.value == rhs.value
            && lhs.domain == rhs.domain
            && lhs.ioType == rhs.ioType
            && lhs.clipMode == rhs.clipMode
            && lhs.unit == rhs.unit
            && lhs.repetitionFilter == rhs.repetitionFilter
            && lhs.rate == rhs.rate
            && lhs.priority == rhs.priority
            && lhs.tags == rhs.tags;
}

inline bool operator!=(
        const Device::AddressSettingsCommon& lhs,
        const Device::AddressSettingsCommon& rhs)
{
    return !(lhs == rhs);
}
inline bool operator==(
        const Device::AddressSettings& lhs,
        const Device::AddressSettings& rhs)
{
    return static_cast<const Device::AddressSettingsCommon&>(lhs) == static_cast<const Device::AddressSettingsCommon&>(rhs)
            && lhs.name == rhs.name;
}

inline bool operator!=(
        const Device::AddressSettings& lhs,
        const Device::AddressSettings& rhs)
{
    return !(lhs == rhs);
}
inline bool operator==(
        const Device::FullAddressSettings& lhs,
        const Device::FullAddressSettings& rhs)
{
    return static_cast<const Device::AddressSettingsCommon&>(lhs) == static_cast<const Device::AddressSettingsCommon&>(rhs)
            && lhs.address == rhs.address;
}

inline bool operator!=(
        const Device::FullAddressSettings& lhs,
        const Device::FullAddressSettings& rhs)
{
    return !(lhs == rhs);
}
}

JSON_METADATA(Device::AddressSettings, "AddressSettings")
