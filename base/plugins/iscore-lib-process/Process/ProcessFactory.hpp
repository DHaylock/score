#pragma once

#include <Process/TimeValue.hpp>
#include <iscore/plugins/customfactory/FactoryInterface.hpp>
#include <QByteArray>
#include <QString>
#include <iscore_lib_process_export.h>
#include <iscore/tools/SettableIdentifier.hpp>

class QGraphicsItem;
class QObject;
struct VisitorVariant;
namespace iscore
{
class DocumentContext;
}
namespace Process
{
class LayerModel;
class LayerPresenter;
class LayerView;
class ProcessModel;
class LayerModelPanelProxy;
struct ProcessPresenterContext;

/**
     * @brief The ProcessFactory class
     *
     * Interface to make processes, like Scenario, Automation...
     */

class ISCORE_LIB_PROCESS_EXPORT ProcessModelFactory:
        public iscore::AbstractFactory<ProcessModelFactory>
{
    ISCORE_ABSTRACT_FACTORY("507ae654-f3b8-4aae-afc3-7ab8e1a3a86f")
    public:
        virtual ~ProcessModelFactory();

        virtual QString prettyName() const = 0;

        virtual Process::ProcessModel* make(
                const TimeValue& duration,
                const Id<ProcessModel>& id,
                QObject* parent) = 0;

        virtual Process::ProcessModel* load(
                const VisitorVariant&,
                QObject* parent) = 0;

};

class ISCORE_LIB_PROCESS_EXPORT LayerFactory:
        public iscore::AbstractFactory<LayerFactory>
{
    ISCORE_ABSTRACT_FACTORY("aeee61e4-89aa-42ec-aa33-bf4522ed710b")
    public:
        virtual ~LayerFactory();

        //// View models interface
        // For deterministic operation in a command,
        // we have to generate some data (like ids...) before making a new view model.
        // This data is valid for construction only for the current state
        // of the scenario.
        virtual QByteArray makeLayerConstructionData(
            const Process::ProcessModel&) const;

        // TODO pass the name of the view model to be created
        // (e.g. temporal / logical...).
        Process::LayerModel* make(
                Process::ProcessModel&,
                const Id<Process::LayerModel>& viewModelId,
                const QByteArray& constructionData,
                QObject* parent);

        // Load
        Process::LayerModel* load(
                const VisitorVariant& v,
                QObject* parent);

        // Clone
        Process::LayerModel* cloneLayer(
                Process::ProcessModel&,
                const Id<Process::LayerModel>& newId,
                const Process::LayerModel& source,
                QObject* parent);

        // The layers may need some specific static data to construct,
        // this provides it (for the sake of commands)
        virtual QByteArray makeStaticLayerConstructionData() const;

        // TODO Make it take a view name, too (cf. logical / temporal).
        // Or make it be created by the ViewModel, and the View be created by the presenter.
        virtual Process::LayerPresenter* makeLayerPresenter(
                const Process::LayerModel&,
                Process::LayerView*,
                const Process::ProcessPresenterContext& context,
                QObject* parent);

        virtual Process::LayerView* makeLayerView(
                const Process::LayerModel& view,
                QGraphicsItem* parent);

        virtual Process::LayerModelPanelProxy* makePanel(
                const LayerModel& layer,
                QObject* parent);

        virtual bool matches(
                const Process::ProcessModel&) const;

    protected:
        virtual LayerModel* makeLayer_impl(
                Process::ProcessModel&,
                const Id<Process::LayerModel>& viewModelId,
                const QByteArray& constructionData,
                QObject* parent);
        virtual LayerModel* loadLayer_impl(
                Process::ProcessModel&,
                const VisitorVariant&,
                QObject* parent);
        virtual LayerModel* cloneLayer_impl(
                Process::ProcessModel&,
                const Id<Process::LayerModel>& newId,
                const Process::LayerModel& source,
                QObject* parent);
};
}
