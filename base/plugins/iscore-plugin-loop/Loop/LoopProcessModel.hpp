#pragma once
#include <Loop/LoopProcessMetadata.hpp>
#include <Process/Process.hpp>
#include <Scenario/Document/BaseScenario/BaseScenarioContainer.hpp>
#include <QByteArray>
#include <QString>
#include <QVector>

#include <Process/ProcessFactoryKey.hpp>
#include <Process/TimeValue.hpp>
#include <iscore/selection/Selection.hpp>
#include <iscore/serialization/VisitorInterface.hpp>

class ConstraintModel;
class DataStream;
class JSONObject;
class LayerModel;
class ProcessStateDataInterface;
class QObject;
class TimeNodeModel;
#include <iscore/tools/SettableIdentifier.hpp>
#include <iscore_plugin_loop_export.h>

namespace Loop
{
class ISCORE_PLUGIN_LOOP_EXPORT ProcessModel final : public Process, public BaseScenarioContainer
{
        ISCORE_METADATA(Loop::ProcessModel)
        ISCORE_SERIALIZE_FRIENDS(Loop::ProcessModel, DataStream)
        ISCORE_SERIALIZE_FRIENDS(Loop::ProcessModel, JSONObject)

    public:
        explicit ProcessModel(
                const TimeValue& duration,
                const Id<Process>& id,
                QObject* parentObject);

        explicit ProcessModel(
                const ProcessModel& source,
                const Id<Process>& id,
                QObject* parentObject);

        template<typename Impl>
        explicit ProcessModel(
                Deserializer<Impl>& vis,
                QObject* parent) :
            Process{vis, parent},
            BaseScenarioContainer{this}
        {
            vis.writeTo(*this);
        }

        using BaseScenarioContainer::event;
        using QObject::event;

        // Process interface
        ProcessModel* clone(
                const Id<Process>& newId,
                QObject* newParent) const override;

        const ProcessFactoryKey& key() const override
        {
            return LoopProcessMetadata::factoryKey();
        }

        QString prettyName() const override;
        QByteArray makeLayerConstructionData() const override;

        void setDurationAndScale(const TimeValue& newDuration) override;
        void setDurationAndGrow(const TimeValue& newDuration) override;
        void setDurationAndShrink(const TimeValue& newDuration) override;

        void startExecution() override;
        void stopExecution() override;
        void reset() override;

        ProcessStateDataInterface* startStateData() const override;
        ProcessStateDataInterface* endStateData() const override;

        Selection selectableChildren() const override;
        Selection selectedChildren() const override;
        void setSelection(const Selection& s) const override;

        void serialize(const VisitorVariant& vis) const override;

    protected:
        LayerModel* makeLayer_impl(const Id<LayerModel>& viewModelId, const QByteArray& constructionData, QObject* parentObject) override;
        LayerModel* loadLayer_impl(const VisitorVariant&, QObject* parentObject) override;
        LayerModel* cloneLayer_impl(const Id<LayerModel>& newId, const LayerModel& source, QObject* parentObject) override;

};


const QVector<Id<ConstraintModel> > constraintsBeforeTimeNode(
        const Loop::ProcessModel& scen,
        const Id<TimeNodeModel>& timeNodeId);
}
