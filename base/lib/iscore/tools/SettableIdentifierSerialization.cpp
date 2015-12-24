#include <boost/optional/optional.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONValueVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <iscore/tools/std/Optional.hpp>
#include <sys/types.h>

template <typename T> class Reader;
template <typename T> class Writer;

// TODO refactor with OptionalValue serialization.
template<>
ISCORE_LIB_BASE_EXPORT void Visitor<Reader<DataStream>>::readFrom(const boost::optional<int32_t>& obj)
{
    m_stream << static_cast<bool>(obj);

    if(obj)
    {
        m_stream << get(obj);
    }
}

template<>
ISCORE_LIB_BASE_EXPORT void Visitor<Writer<DataStream>>::writeTo(boost::optional<int32_t>& obj)
{
    bool b {};
    m_stream >> b;

    if(b)
    {
        int32_t val;
        m_stream >> val;

        obj = val;
    }
    else
    {
        reset(obj);
    }
}

// TODO should not be used. Save as optional json value instead.
template<>
ISCORE_LIB_BASE_EXPORT void Visitor<Reader<JSONObject>>::readFrom(const boost::optional<int32_t>& obj)
{
    if(obj)
    {
        m_obj["id"] = get(obj);
    }
    else
    {
        m_obj["id"] = "none";
    }
}

template<>
ISCORE_LIB_BASE_EXPORT void Visitor<Writer<JSONObject>>::writeTo(boost::optional<int32_t>& obj)
{
    if(m_obj["id"].toString() == "none")
    {
        reset(obj);
    }
    else
    {
        obj = m_obj["id"].toInt();
    }
}


template<>
ISCORE_LIB_BASE_EXPORT void Visitor<Reader<JSONValue>>::readFrom(const boost::optional<int32_t>& obj)
{
    if(obj)
    {
        val = get(obj);
    }
    else
    {
        val = "none";
    }
}

template<>
ISCORE_LIB_BASE_EXPORT void Visitor<Writer<JSONValue>>::writeTo(boost::optional<int32_t>& obj)
{
    if(val.toString() == "none")
    {
        reset(obj);
    }
    else
    {
        obj = val.toInt();
    }
}
