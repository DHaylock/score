#pragma once
#include <Process/LayerModel.hpp>

#include <iscore/tools/Metadata.hpp>
#include <iscore/serialization/VisitorInterface.hpp>

#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <iscore/tools/SettableIdentifier.hpp>

namespace Scenario
{
class TemporalConstraintViewModel;
}

namespace Loop{
    class ProcessModel;

class Layer final : public Process::LayerModel
{
        ISCORE_SERIALIZE_FRIENDS(Layer, DataStream)
        ISCORE_SERIALIZE_FRIENDS(Layer, JSONObject)

        Q_OBJECT
    public:
        Layer(
                Loop::ProcessModel& model,
                const Id<LayerModel>& id,
                QObject* parent);

        // Copy
        Layer(
                const Layer& source,
                Loop::ProcessModel& model,
                const Id<LayerModel>& id,
                QObject* parent);

        // Load
        template<typename Impl>
        Layer(
                Deserializer<Impl>& vis,
                Loop::ProcessModel& model,
                QObject* parent) :
            LayerModel {vis, model, parent}
        {
            vis.writeTo(*this);
        }

        void serialize_impl(const VisitorVariant&) const override;

        const Loop::ProcessModel& model() const;

        const Scenario::TemporalConstraintViewModel& constraint() const
        {
            return *m_constraint;
        }

    private:
        Scenario::TemporalConstraintViewModel* m_constraint{};
};
}

DEFAULT_MODEL_METADATA(Loop::Layer, "Loop layer")
