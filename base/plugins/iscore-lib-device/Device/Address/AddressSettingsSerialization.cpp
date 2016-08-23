#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <QDataStream>
#include <QtGlobal>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QMap>
#include <QString>
#include <QStringList>

#include "AddressSettings.hpp"
#include <Device/Address/ClipMode.hpp>
#include <Device/Address/Domain.hpp>
#include <Device/Address/IOType.hpp>
#include "DomainSerialization.hpp"
#include <State/Value.hpp>

template <typename T> class Reader;
template <typename T> class Writer;


template<>
void Visitor<Reader<DataStream>>::readFrom(const Device::AddressSettingsCommon& n)
{
    m_stream << n.value
             << n.domain
             << n.ioType
             << n.clipMode
             << n.unit
             << n.repetitionFilter
             << n.rate
             << n.priority
             << n.tags;
}


template<>
void Visitor<Writer<DataStream>>::writeTo(Device::AddressSettingsCommon& n)
{
    m_stream >> n.value
             >> n.domain
             >> n.ioType
             >> n.clipMode
             >> n.unit
             >> n.repetitionFilter
             >> n.rate
             >> n.priority
             >> n.tags;
}


template<>
void Visitor<Reader<JSONObject>>::readFrom(const Device::AddressSettingsCommon& n)
{
    // Metadata
    m_obj[strings.ioType] = Device::IOTypeStringMap()[n.ioType];
    m_obj[strings.ClipMode] = Device::ClipModeStringMap()[n.clipMode];
    m_obj[strings.Unit] = n.unit;

    m_obj[strings.RepetitionFilter] = n.repetitionFilter;
    m_obj[strings.RefreshRate] = n.rate;

    m_obj[strings.Priority] = n.priority;

    QJsonArray arr;
    for(auto& str : n.tags)
        arr.append(str);
    m_obj[strings.Tags] = arr;

    // Value, domain and type
    readFrom(n.value);
    m_obj[strings.Domain] = DomainToJson(n.domain);
}


template<>
void Visitor<Writer<JSONObject>>::writeTo(Device::AddressSettingsCommon& n)
{
    n.ioType = Device::IOTypeStringMap().key(m_obj[strings.ioType].toString());
    n.clipMode = Device::ClipModeStringMap().key(m_obj[strings.ClipMode].toString());
    n.unit = m_obj[strings.Unit].toString();

    n.repetitionFilter = m_obj[strings.RepetitionFilter].toBool();
    n.rate = m_obj[strings.RefreshRate].toInt();

    n.priority = m_obj[strings.Priority].toInt();

    auto arr = m_obj[strings.Tags].toArray();
    for(auto&& elt : arr)
        n.tags.append(elt.toString());

    writeTo(n.value);
    // TODO doesn't handle multi-type variants.
    if(m_obj.contains(strings.Type))
    {
        n.domain = Device::JsonToDomain(
            m_obj[strings.Domain].toObject(),
            m_obj[strings.Type].toString());
    }
}


template<>
ISCORE_LIB_DEVICE_EXPORT void Visitor<Reader<DataStream>>::readFrom(const Device::AddressSettings& n)
{
    readFrom(static_cast<const Device::AddressSettingsCommon&>(n));
    m_stream << n.name;

    insertDelimiter();
}
template<>
ISCORE_LIB_DEVICE_EXPORT void Visitor<Writer<DataStream>>::writeTo(Device::AddressSettings& n)
{
    writeTo(static_cast<Device::AddressSettingsCommon&>(n));
    m_stream >> n.name;

    checkDelimiter();
}
template<>
ISCORE_LIB_DEVICE_EXPORT void Visitor<Reader<DataStream>>::readFrom(const Device::FullAddressSettings& n)
{
    readFrom(static_cast<const Device::AddressSettingsCommon&>(n));
    m_stream << n.address;

    insertDelimiter();
}
template<>
ISCORE_LIB_DEVICE_EXPORT void Visitor<Writer<DataStream>>::writeTo(Device::FullAddressSettings& n)
{
    writeTo(static_cast<Device::AddressSettingsCommon&>(n));
    m_stream >> n.address;

    checkDelimiter();
}

template<>
ISCORE_LIB_DEVICE_EXPORT void Visitor<Reader<JSONObject>>::readFrom(const Device::AddressSettings& n)
{
    readFrom(static_cast<const Device::AddressSettingsCommon&>(n));
    m_obj[strings.Name] = n.name;
}

template<>
ISCORE_LIB_DEVICE_EXPORT void Visitor<Writer<JSONObject>>::writeTo(Device::AddressSettings& n)
{
    writeTo(static_cast<Device::AddressSettingsCommon&>(n));
    n.name = m_obj[strings.Name].toString();
}

template<>
ISCORE_LIB_DEVICE_EXPORT void Visitor<Reader<JSONObject>>::readFrom(const Device::FullAddressSettings& n)
{
    readFrom(static_cast<const Device::AddressSettingsCommon&>(n));
    m_obj[strings.Address] = toJsonObject(n.address);
}

template<>
ISCORE_LIB_DEVICE_EXPORT void Visitor<Writer<JSONObject>>::writeTo(Device::FullAddressSettings& n)
{
    writeTo(static_cast<Device::AddressSettingsCommon&>(n));
    n.address = fromJsonObject<State::Address>(m_obj[strings.Address]);
}
