#include <Device/Node/DeviceNode.hpp>
#include <Explorer/Explorer/DeviceExplorerModel.hpp>
#include <Mapping/MappingModel.hpp>
#include <QString>
#include <QStringList>
#include <algorithm>

#include "ChangeAddresses.hpp"
#include <Device/Address/AddressSettings.hpp>
#include <Device/Address/Domain.hpp>
#include <State/Address.hpp>
#include <State/Value.hpp>
#include <State/ValueConversion.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/tools/ModelPath.hpp>
#include <iscore/tools/ModelPathSerialization.hpp>
#include <iscore/tools/TreeNode.hpp>

// TODO try to template this to reuse it with ChangeAddress / ChangeTargetAddress
// TODO why not use AddressSettings directly on Automations / Mapping ? It would simplify...
ChangeSourceAddress::ChangeSourceAddress(
        Path<MappingModel> &&path,
        const State::Address &newval):
    m_path{path}
{
    auto& mapping = m_path.find();
    auto& deviceexplorer = deviceExplorerFromObject(mapping);

    // Note : since we change the address, we also have to update the min / max if possible.
    // To do this, we must go and check into the device explorer.
    // If the node isn't found, we fallback on common values.

    // Get the current data.
    m_old.address = mapping.sourceAddress();
    m_old.domain.min.val = mapping.sourceMin();
    m_old.domain.max.val = mapping.sourceMax();

    // Get the new data.
    auto newpath = newval.path;
    newpath.prepend(newval.device);
    auto new_n = Device::try_getNodeFromString(deviceexplorer.rootNode(), std::move(newpath));
    if(new_n)
    {
        ISCORE_ASSERT(!new_n->is<Device::DeviceSettings>());
        m_new = Device::FullAddressSettings::make<Device::FullAddressSettings::as_child>(new_n->get<Device::AddressSettings>(), newval);
    }
    else
    {
        m_new.address = newval;
        m_new.domain.min.val = 0.;
        m_new.domain.max.val = 1.;
    }
}


void ChangeSourceAddress::undo() const
{
    auto& mapping = m_path.find();

    mapping.setSourceMin(State::convert::value<double>(m_old.domain.min));
    mapping.setSourceMax(State::convert::value<double>(m_old.domain.max));

    mapping.setSourceAddress(m_old.address);
}

void ChangeSourceAddress::redo() const
{
    auto& mapping = m_path.find();

    mapping.setSourceMin(State::convert::value<double>(m_new.domain.min));
    mapping.setSourceMax(State::convert::value<double>(m_new.domain.max));

    mapping.setSourceAddress(m_new.address);
}

void ChangeSourceAddress::serializeImpl(DataStreamInput & s) const
{
    s << m_path << m_old << m_new;
}

void ChangeSourceAddress::deserializeImpl(DataStreamOutput & s)
{
    s >> m_path >> m_old >> m_new;
}






ChangeTargetAddress::ChangeTargetAddress(
        Path<MappingModel> &&path,
        const State::Address &newval):
    m_path{path}
{
    auto& mapping = m_path.find();
    auto& deviceexplorer = deviceExplorerFromObject(mapping);

    // Note : since we change the address, we also have to update the min / max if possible.
    // To do this, we must go and check into the device explorer.
    // If the node isn't found, we fallback on common values.

    // Get the current data.
    m_old.address = mapping.targetAddress();
    m_old.domain.min.val = mapping.targetMin();
    m_old.domain.max.val = mapping.targetMax();

    // Get the new data.
    auto newpath = newval.path;
    newpath.prepend(newval.device);
    auto new_n = Device::try_getNodeFromString(deviceexplorer.rootNode(), std::move(newpath));
    if(new_n)
    {
        ISCORE_ASSERT(!new_n->is<Device::DeviceSettings>());
        m_new = Device::FullAddressSettings::make<Device::FullAddressSettings::as_child>(new_n->get<Device::AddressSettings>(), newval);
    }
    else
    {
        m_new.address = newval;
        m_new.domain.min.val = 0.;
        m_new.domain.max.val = 1.;
    }
}


void ChangeTargetAddress::undo() const
{
    auto& mapping = m_path.find();

    mapping.setTargetMin(State::convert::value<double>(m_old.domain.min));
    mapping.setTargetMax(State::convert::value<double>(m_old.domain.max));

    mapping.setTargetAddress(m_old.address);
}

void ChangeTargetAddress::redo() const
{
    auto& mapping = m_path.find();

    mapping.setTargetMin(State::convert::value<double>(m_new.domain.min));
    mapping.setTargetMax(State::convert::value<double>(m_new.domain.max));

    mapping.setTargetAddress(m_new.address);
}

void ChangeTargetAddress::serializeImpl(DataStreamInput & s) const
{
    s << m_path << m_old << m_new;
}

void ChangeTargetAddress::deserializeImpl(DataStreamOutput & s)
{
    s >> m_path >> m_old >> m_new;
}
