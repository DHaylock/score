// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>

#include "LocalSpecificSettings.hpp"


template <>
void DataStreamReader::read(
    const Engine::Network::LocalSpecificSettings& n)
{
  m_stream << n.wsPort << n.oscPort;
  insertDelimiter();
}


template <>
void DataStreamWriter::write(
    Engine::Network::LocalSpecificSettings& n)
{
  m_stream >> n.wsPort >> n.oscPort;
  checkDelimiter();
}


template <>
void JSONObjectReader::read(
    const Engine::Network::LocalSpecificSettings& n)
{
  obj["WSPort"] = n.wsPort;
  obj["OSCport"] = n.oscPort;
}


template <>
void JSONObjectWriter::write(
    Engine::Network::LocalSpecificSettings& n)
{
  n.wsPort = obj["WSPort"].toInt();
  n.oscPort = obj["OSCPort"].toInt();
}
