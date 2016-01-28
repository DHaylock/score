#include <iscore/serialization/DataStreamVisitor.hpp>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

template <typename T> class Reader;
template <typename T> class Writer;

template<>
void Visitor<Reader<DataStream>>::readFrom(const QJsonObject& obj)
{
    QJsonDocument doc{obj};
    m_stream << doc.toBinaryData();
    insertDelimiter();
}
template<>
void Visitor<Writer<DataStream>>::writeTo(QJsonObject& obj)
{
    QByteArray arr;
    m_stream >> arr;

    obj = QJsonDocument::fromBinaryData(arr).object();

    checkDelimiter();
}
