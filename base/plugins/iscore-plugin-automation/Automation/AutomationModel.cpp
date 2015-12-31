#include <boost/optional/optional.hpp>
#include <iscore/document/DocumentInterface.hpp>
#include <QDebug>
#include <QPoint>

#include <Automation/AutomationProcessMetadata.hpp>
#include "AutomationLayerModel.hpp"
#include "AutomationModel.hpp"
#include <Curve/CurveModel.hpp>
#include <Curve/Palette/CurvePoint.hpp>
#include <Curve/Process/CurveProcessModel.hpp>
#include <Curve/Segment/CurveSegmentModel.hpp>
#include <Curve/Segment/Power/PowerCurveSegmentModel.hpp>
#include <Process/ModelMetadata.hpp>
#include <State/Address.hpp>
#include <Automation/State/AutomationState.hpp>
#include <iscore/plugins/documentdelegate/plugin/ElementPluginModelList.hpp>
#include <iscore/tools/IdentifiedObjectMap.hpp>
#include <iscore/tools/SettableIdentifier.hpp>

namespace Process { class LayerModel; }
namespace Process { class ProcessModel; }
class QObject;

AutomationModel::AutomationModel(
        const TimeValue& duration,
        const Id<ProcessModel>& id,
        QObject* parent) :
    CurveProcessModel {duration, id, AutomationProcessMetadata::processObjectName(), parent},
    m_startState{new AutomationState{*this, 0., this}},
    m_endState{new AutomationState{*this, 1., this}}
{
    pluginModelList = new iscore::ElementPluginModelList{iscore::IDocument::documentContext(*parent), this};

    // Named shall be enough ?
    setCurve(new Curve::Model{Id<Curve::Model>(45345), this});

    auto s1 = new Curve::DefaultCurveSegmentModel(Id<Curve::SegmentModel>(1), m_curve);
    s1->setStart({0., 0.0});
    s1->setEnd({1., 1.});

    m_curve->addSegment(s1);
    connect(m_curve, &Curve::Model::changed,
            this, &AutomationModel::curveChanged);

    metadata.setName(QString("Automation.%1").arg(*this->id().val()));
}

AutomationModel::AutomationModel(
        const AutomationModel& source,
        const Id<ProcessModel>& id,
        QObject* parent):
    CurveProcessModel{source, id, AutomationProcessMetadata::processObjectName(), parent},
    m_address(source.address()),
    m_min{source.min()},
    m_max{source.max()},
    m_startState{new AutomationState{*this, 0., this}},
    m_endState{new AutomationState{*this, 1., this}}
{
    setCurve(source.curve().clone(source.curve().id(), this));
    pluginModelList = new iscore::ElementPluginModelList(*source.pluginModelList, this);
    connect(m_curve, &Curve::Model::changed,
            this, &AutomationModel::curveChanged);
    metadata.setName(QString("Automation.%1").arg(*this->id().val()));
}

Process::ProcessModel* AutomationModel::clone(
        const Id<ProcessModel>& newId,
        QObject* newParent) const
{
    return new AutomationModel {*this, newId, newParent};
}

const ProcessFactoryKey& AutomationModel::key() const
{
    return AutomationProcessMetadata::factoryKey();
}

QString AutomationModel::prettyName() const
{
    return metadata.name() + " : " + address().toString();
}

void AutomationModel::setDurationAndScale(const TimeValue& newDuration)
{
    // We only need to change the duration.
    setDuration(newDuration);
    m_curve->changed();
}

void AutomationModel::setDurationAndGrow(const TimeValue& newDuration)
{
    // If there are no segments, nothing changes
    if(m_curve->segments().size() == 0)
    {
        setDuration(newDuration);
        return;
    }

    // Else, scale all the segments by the increase.
    double scale = duration() / newDuration;
    for(auto& segment : m_curve->segments())
    {
        Curve::Point pt = segment.start();
        pt.setX(pt.x() * scale);
        segment.setStart(pt);

        pt = segment.end();
        pt.setX(pt.x() * scale);
        segment.setEnd(pt);
    }

    setDuration(newDuration);
    m_curve->changed();
}

void AutomationModel::setDurationAndShrink(const TimeValue& newDuration)
{
    // If there are no segments, nothing changes
    if(m_curve->segments().size() == 0)
    {
        setDuration(newDuration);
        return;
    }

    // Else, scale all the segments by the increase.
    double scale = duration() / newDuration;
    for(auto& segment : m_curve->segments())
    {
        Curve::Point pt = segment.start();
        pt.setX(pt.x() * scale);
        segment.setStart(pt);

        pt = segment.end();
        pt.setX(pt.x() * scale);
        segment.setEnd(pt);
    }

    // Since we shrink, scale > 1. so we have to cut.
    // Note:  this will certainly change how some functions do look.
    auto segments = m_curve->segments(); // Make a copy since we will change the map.
    for(auto& segment : segments)
    {
        if(segment.start().x() >= 1.)
        {
            // bye
            m_curve->removeSegment(&segment);
        }
        else if(segment.end().x() >= 1.)
        {
            auto end = segment.end();
            end.setX(1.);
            segment.setEnd(end);
        }
    }

    setDuration(newDuration);
    m_curve->changed();
}

Process::LayerModel* AutomationModel::makeLayer_impl(
        const Id<Process::LayerModel>& viewModelId,
        const QByteArray& constructionData,
        QObject* parent)
{
    auto vm = new AutomationLayerModel{*this, viewModelId, parent};
    return vm;
}

Process::LayerModel* AutomationModel::cloneLayer_impl(
        const Id<Process::LayerModel>& newId,
        const Process::LayerModel& source,
        QObject* parent)
{
    auto vm = new AutomationLayerModel {
              static_cast<const AutomationLayerModel&>(source), *this, newId, parent};
    return vm;
}

void AutomationModel::setCurve_impl()
{
    connect(m_curve, &Curve::Model::changed,
            this, [&] () {
        emit curveChanged();

        m_startState->messagesChanged(m_startState->messages());
        m_endState->messagesChanged(m_endState->messages());
    });
}


AutomationState* AutomationModel::startStateData() const
{
    return m_startState;
}

AutomationState* AutomationModel::endStateData() const
{
    return m_endState;
}

iscore::Address AutomationModel::address() const
{
    return m_address;
}

double AutomationModel::value(const TimeValue& time)
{
    ISCORE_TODO;
    // TODO instead get a State or at least a MessageList.
    return -1;
}

double AutomationModel::min() const
{
    return m_min;
}

double AutomationModel::max() const
{
    return m_max;
}

void AutomationModel::setAddress(const iscore::Address &arg)
{
    if(m_address == arg)
    {
        return;
    }

    m_address = arg;
    emit addressChanged(arg);
    emit m_curve->changed();
}

void AutomationModel::setMin(double arg)
{
    if (m_min == arg)
        return;

    m_min = arg;
    emit minChanged(arg);
    emit m_curve->changed();
}

void AutomationModel::setMax(double arg)
{
    if (m_max == arg)
        return;

    m_max = arg;
    emit maxChanged(arg);
    emit m_curve->changed();
}

