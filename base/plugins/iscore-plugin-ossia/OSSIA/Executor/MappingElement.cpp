#include "MappingElement.hpp"

#include <API/Headers/Editor/Mapper.h>
#include <API/Headers/Editor/State.h>
#include <API/Headers/Editor/Message.h>
#include <API/Headers/Editor/Value.h>
#include <Mapping/MappingModel.hpp>
#include <OSSIA/iscore2OSSIA.hpp>

#include <iscore/document/DocumentInterface.hpp>
#include <core/document/Document.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <OSSIA/Protocols/OSSIADevice.hpp>
#include "ConstraintElement.hpp"

#include <Curve/CurveModel.hpp>
#include <Curve/Segment/Linear/LinearCurveSegmentModel.hpp>
#include <Curve/Segment/Power/PowerCurveSegmentModel.hpp>

#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <core/document/Document.hpp>
#include <OSSIA/iscore2OSSIA.hpp>
#include <OSSIA/Protocols/OSSIADevice.hpp>
#include <API/Headers/Editor/Curve.h>
#include <API/Headers/Editor/CurveSegment/CurveSegmentLinear.h>
#include <API/Headers/Editor/CurveSegment/CurveSegmentPower.h>
#include <OSSIA/Executor/ExecutorContext.hpp>
#include <OSSIA/Executor/DocumentPlugin.hpp>
namespace RecreateOnPlay
{
MappingElement::MappingElement(
        ConstraintElement& parentConstraint,
        MappingModel& element,
        const Context& ctx,
        const Id<iscore::Component>& id,
        QObject *parent):
    ProcessComponent{parentConstraint, element, id, "MappingElement", parent},
    m_iscore_mapping{element},
    m_deviceList{ctx.devices}
{
    recreate();

}

std::shared_ptr<OSSIA::TimeProcess> MappingElement::OSSIAProcess() const
{
    return {}; //m_ossia_mapping;
}

Process::ProcessModel& MappingElement::iscoreProcess() const
{
    return m_iscore_mapping;
}


const MappingElement::Key& MappingElement::key() const
{
    static iscore::Component::Key k("OSSIAMappingElement");
    return k;
}

template<typename X_T, typename Y_T>
std::shared_ptr<OSSIA::CurveAbstract> MappingElement::on_curveChanged_impl2()
{
    if(m_iscore_mapping.curve().segments().size() == 0)
        return {};

    const double xmin = m_iscore_mapping.sourceMin();
    const double xmax = m_iscore_mapping.sourceMax();

    const double ymin = m_iscore_mapping.targetMin();
    const double ymax = m_iscore_mapping.targetMax();

    auto scale_x = [=] (double val) -> X_T { return val * (xmax - xmin) + xmin; };
    auto scale_y = [=] (double val) -> Y_T { return val * (ymax - ymin) + ymin; };

    auto segt_data = m_iscore_mapping.curve().toCurveData();

    if(segt_data.size() != 0)
    {
        std::sort(segt_data.begin(), segt_data.end());
        return iscore::convert::curve<X_T, Y_T>(scale_x, scale_y, segt_data);
    }
    else
    {
        return {};
    }
}

template<typename X_T>
std::shared_ptr<OSSIA::CurveAbstract> MappingElement::on_curveChanged_impl()
{
    switch(m_targetAddressType)
    {
        case OSSIA::Value::Type::INT:
            return on_curveChanged_impl2<X_T, int>();
            break;
        case OSSIA::Value::Type::FLOAT:
            return on_curveChanged_impl2<X_T, float>();
            break;
        default:
            qDebug() << "Unsupported target address type: " << (int)m_targetAddressType;
            ISCORE_TODO;
    }

    return {};
}

std::shared_ptr<OSSIA::CurveAbstract> MappingElement::rebuildCurve()
{
    m_ossia_curve.reset();
    switch(m_sourceAddressType)
    {
        case OSSIA::Value::Type::INT:
            m_ossia_curve = on_curveChanged_impl<int>();
            break;
        case OSSIA::Value::Type::FLOAT:
            m_ossia_curve = on_curveChanged_impl<float>();
            break;
        default:
            qDebug() << "Unsupported source address type: " << (int)m_sourceAddressType;
            ISCORE_TODO;
    }

    return m_ossia_curve;
}

void MappingElement::recreate()
{
    auto iscore_source_addr = m_iscore_mapping.sourceAddress();
    auto iscore_target_addr = m_iscore_mapping.targetAddress();

    std::shared_ptr<OSSIA::Address> ossia_source_addr;
    std::shared_ptr<OSSIA::Address> ossia_target_addr;
    std::shared_ptr<OSSIA::Mapper> new_mapping;


    m_ossia_curve.reset(); // It will be remade after.

    // Get the device list to obtain the nodes.
    const auto& devices = m_deviceList.devices();

    // TODO use this in automation
    auto getAddress = [&] (const State::Address& addr) -> std::shared_ptr<OSSIA::Address>
    {
        // Look for the real node in the device
        auto dev_it = std::find_if(devices.begin(), devices.end(),
                                   [&] (DeviceInterface* dev) {
            return dev->settings().name == addr.device;
        });

        if(dev_it == devices.end())
            return {};

        auto dev = dynamic_cast<OSSIADevice*>(*dev_it);
        if(!dev)
            return {};

        auto node = iscore::convert::findNodeFromPath(addr.path, &dev->impl());
        if(!node)
            return {};

        // Add the real address
        auto address = node->getAddress();
        if(!address)
            return {};
        return address;
    };

    ossia_source_addr = getAddress(iscore_source_addr);
    if(!ossia_source_addr)
        goto curve_cleanup_label;

    m_sourceAddressType = ossia_source_addr->getValueType();

    ossia_target_addr = getAddress(iscore_target_addr);
    if(!ossia_target_addr)
        goto curve_cleanup_label;

    m_targetAddressType = ossia_target_addr->getValueType();


    using namespace OSSIA;
    rebuildCurve(); // If the type changes we need to rebuild the curve.
    if(!m_ossia_curve)
        goto curve_cleanup_label;

    // TODO on_min/max changed
    m_ossia_mapping = OSSIA::Mapper::create(
                ossia_source_addr, ossia_target_addr,
                new Behavior(m_ossia_curve));

    return;

curve_cleanup_label:
    m_ossia_mapping.reset();
    return;
}




MappingComponentFactory::~MappingComponentFactory()
{

}

ProcessComponent* MappingComponentFactory::make(
        ConstraintElement& cst,
        Process::ProcessModel& proc,
        const Context& ctx,
        const Id<iscore::Component>& id,
        QObject* parent) const
{

    return new MappingElement{
                cst,
                static_cast<MappingModel&>(proc),
                ctx, id, parent};

}

const MappingComponentFactory::factory_key_type&
MappingComponentFactory::key_impl() const
{
    static MappingComponentFactory::factory_key_type k("OSSIAMappingElement");
    return k;
}

bool MappingComponentFactory::matches(
        Process::ProcessModel& proc,
        const DocumentPlugin&,
        const iscore::DocumentContext&) const
{
    return dynamic_cast<MappingModel*>(&proc);
}
}
