#pragma once
#include <iscore/serialization/VisitorInterface.hpp>
#include <iscore/tools/SettableIdentifier.hpp>
#include <QJsonValue>
#include <QJsonArray>
#include <QMap>

template<class>
class StringKey;

template<typename T>
T fromJsonObject(QJsonValue&& json);

class JSONValue;
template<> class Visitor<Reader<JSONValue>>;
template<> class Visitor<Writer<JSONValue>>;

class JSONValue
{
    public:
        using Serializer = Visitor<Reader<JSONValue>>;
        using Deserializer = Visitor<Writer<JSONValue>>;
        static constexpr SerializationIdentifier type()
        {
            return 3;
        }
};

template<>
class ISCORE_LIB_BASE_EXPORT Visitor<Reader<JSONValue>> final : public AbstractVisitor
{
    public:
        using is_visitor_tag = std::integral_constant<bool, true>;

        Visitor<Reader<JSONValue>>() = default;
        Visitor<Reader<JSONValue>>(const Visitor<Reader<JSONValue>>&) = delete;
        Visitor<Reader<JSONValue>>& operator=(const Visitor<Reader<JSONValue>>&) = delete;

        VisitorVariant toVariant() { return {*this, JSONValue::type()}; }

        template<typename T, std::enable_if_t<!std::is_enum<T>::value>* = nullptr>
        void readFrom(const T&);

        template<typename T, std::enable_if_t<std::is_enum<T>::value>* = nullptr>
        void readFrom(const T& elt)
        {
            val = (int32_t) elt;
        }

        template<typename T>
        void readFrom(const Id<T>& obj)
        {
            readFrom(obj.val());
        }

        template<typename T>
        void readFrom(const StringKey<T>&);

        QJsonValue val;
};

template<>
class ISCORE_LIB_BASE_EXPORT Visitor<Writer<JSONValue>> : public AbstractVisitor
{
    public:
        using is_visitor_tag = std::integral_constant<bool, true>;

        VisitorVariant toVariant() { return {*this, JSONValue::type()}; }

        Visitor<Writer<JSONValue>>() = default;
        Visitor<Writer<JSONValue>>(const Visitor<Reader<JSONValue>>&) = delete;
        Visitor<Writer<JSONValue>>& operator=(const Visitor<Writer<JSONValue>>&) = delete;

        Visitor<Writer<JSONValue>> (const QJsonValue& obj) :
                               val{obj} {}

        Visitor<Writer<JSONValue>> (QJsonValue&& obj) :
                               val{std::move(obj) }  {}

        template<typename T, std::enable_if_t<!std::is_enum<T>::value>* = nullptr>
        void writeTo(T&);

        template<typename T, std::enable_if_t<std::is_enum<T>::value>* = nullptr>
        void writeTo(T& elt)
        {
            elt = static_cast<T>(val.toInt());
        }

        template<typename T>
        void writeTo(StringKey<T>&);

        template<typename T>
        void writeTo(Id<T>& obj)
        {
            typename Id<T>::value_type id_impl;
            writeTo(id_impl);
            obj.setVal(std::move(id_impl));
        }

        QJsonValue val;
};


template<typename T>
QJsonValue toJsonValue(const T& obj)
{
    Visitor<Reader<JSONValue>> reader;
    reader.readFrom(obj);

    return reader.val;
}


template<typename T>
void fromJsonValue(QJsonValue&& json, T& val)
{
    Visitor<Writer<JSONValue>> writer {json};
    writer.writeTo(val);
}

template<typename T>
void fromJsonValue(QJsonValue& json, T& val)
{
    Visitor<Writer<JSONValue>> writer {json};
    writer.writeTo(val);
}

template<typename T>
T fromJsonValue(const QJsonValue& json)
{
    T val;
    Visitor<Writer<JSONValue>> writer {json};
    writer.writeTo(val);
    return val;
}

template<typename T>
void fromJsonValue(QJsonValueRef&& json, T& val)
{
    Visitor<Writer<JSONValue>> writer {static_cast<QJsonValue>(json)};
    writer.writeTo(val);
}

template<typename T>
void fromJsonValue(const QJsonValueRef& json, T& val)
{
    Visitor<Writer<JSONValue>> writer {static_cast<const QJsonValue&>(json)};
    writer.writeTo(val);
}

template<typename T>
T fromJsonValue(QJsonValueRef&& json)
{
    T val;
    Visitor<Writer<JSONValue>> writer {static_cast<QJsonValue>(json)};
    writer.writeTo(val);
    return val;
}

template<typename T>
T fromJsonValue(const QJsonValueRef& json)
{
    T val;
    Visitor<Writer<JSONValue>> writer {static_cast<const QJsonValue&>(json)};
    writer.writeTo(val);
    return val;
}

template<template<typename U> class T, typename V>
void fromJsonValueArray(const QJsonArray&& json_arr, T<Id<V>>& arr)
{
    for(const auto& elt : json_arr)
    {
        arr.push_back(fromJsonValue<Id<V>>(elt));
    }
}


template<typename Container>
QJsonArray toJsonValueArray(const Container& c)
{
    QJsonArray arr;

    for(const auto& elt : c)
    {
        arr.push_back(toJsonValue(elt));
    }

    return arr;
}


template<typename Container>
Container fromJsonValueArray(const QJsonArray& json_arr)
{
    Container c;
    c.reserve(json_arr.size());

    for(const auto& elt : json_arr)
    {
        c.push_back(fromJsonValue<typename Container::value_type>(elt));
    }

    return c;
}
