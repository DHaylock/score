#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>
#include <iscore/plugins/documentdelegate/plugin/DocumentDelegatePluginModel.hpp>
#include <QJsonObject>
#include <QJsonValue>
#include <algorithm>
#include <vector>

#include "ElementPluginModel.hpp"
#include "ElementPluginModelSerialization.hpp"
#include <iscore/document/DocumentInterface.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <iscore/tools/std/StdlibWrapper.hpp>
#include <iscore/tools/std/Algorithms.hpp>

class QObject;

template<>
void Visitor<Reader<DataStream>>::readFrom(const iscore::ElementPluginModel& elt)
{
    m_stream << elt.elementPluginId();
    elt.serialize(this->toVariant());

    insertDelimiter();
}

template<>
void Visitor<Reader<JSONObject>>::readFrom(const iscore::ElementPluginModel& elt)
{
    m_obj["PluginId"] = elt.elementPluginId();
    elt.serialize(this->toVariant());
}

template<>
iscore::ElementPluginModel* deserializeElementPluginModel(
        Deserializer<DataStream>& deserializer,
        const QObject* element,
        QObject* parent)
{
    int pluginId;
    deserializer.m_stream >> pluginId;

    iscore::Document* doc = iscore::IDocument::documentFromObject(parent);

    iscore::ElementPluginModel* model{};
    for(auto& plugin : doc->model().pluginModels())
    {
        if(contains(plugin->elementPlugins(), pluginId))
        {
            model = plugin->loadElementPlugin(
                        element,
                        deserializer.toVariant(),
                        parent);
            if(model)
                break;
            else
                ISCORE_ABORT;
        }
    }

    deserializer.checkDelimiter();
    return model;
}


template<>
iscore::ElementPluginModel* deserializeElementPluginModel(
        Deserializer<JSONObject>& deserializer,
        const QObject* element,
        QObject* parent)
{
    int pluginId = deserializer.m_obj["PluginId"].toInt();

    iscore::Document* doc = iscore::IDocument::documentFromObject(parent);

    iscore::ElementPluginModel* model{};
    for(auto& plugin : doc->model().pluginModels())
    {
        if(contains(plugin->elementPlugins(), pluginId))
        {
            model = plugin->loadElementPlugin(
                        element,
                        deserializer.toVariant(),
                        parent);

            if(model)
                break;
            else
                ISCORE_ABORT;
        }
    }

    return model;
}

