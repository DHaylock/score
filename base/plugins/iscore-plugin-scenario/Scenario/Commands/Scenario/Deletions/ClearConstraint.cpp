#include <Process/ProcessList.hpp>

#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/Constraint/Rack/RackModel.hpp>
#include <Scenario/Document/Constraint/ViewModels/ConstraintViewModel.hpp>

#include <Scenario/Process/Algorithms/ProcessPolicy.hpp>

#include <QDataStream>
#include <QtGlobal>
#include <algorithm>

#include "ClearConstraint.hpp"
#include <Process/Process.hpp>
#include <iscore/application/ApplicationContext.hpp>
#include <iscore/plugins/customfactory/StringFactoryKey.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/tools/IdentifiedObject.hpp>
#include <iscore/tools/ModelPath.hpp>
#include <iscore/tools/MapCopy.hpp>
#include <iscore/tools/ModelPathSerialization.hpp>
#include <iscore/tools/NotifyingMap.hpp>

namespace Scenario
{
namespace Command
{
ClearConstraint::ClearConstraint(Path<ConstraintModel>&& constraintPath) :
    m_path {std::move(constraintPath) }
{
    auto& constraint = m_path.find();

    for(const auto& rack : constraint.racks)
    {
        QByteArray arr;
        Serializer<DataStream> s {&arr};
        s.readFrom(rack);
        m_serializedRackes.push_back(arr);
    }

    for(const auto& process : constraint.processes)
    {
        QByteArray arr;
        Serializer<DataStream> s {&arr};
        s.readFrom(process);
        m_serializedProcesses.push_back(arr);
    }

    // TODO save view model data instead, since later it
    // might be more than just the shown rack.
    for(const auto& viewmodel : constraint.viewModels())
    {
        m_rackMappings.insert(viewmodel->id(), viewmodel->shownRack());
    }
}

void ClearConstraint::undo() const
{
    auto& fact = context.components.factory<Process::ProcessList>();

    auto& constraint = m_path.find();

    for(auto& serializedProcess : m_serializedProcesses)
    {
        Deserializer<DataStream> s {serializedProcess};
        AddProcess(constraint, deserialize_interface(fact, s, &constraint));
    }

    for(auto& serializedRack : m_serializedRackes)
    {
        Deserializer<DataStream> s {serializedRack};
        constraint.racks.add(new RackModel {s, &constraint});
    }

    auto bit = constraint.viewModels().begin(), eit = constraint.viewModels().end();
    for(const auto& cvmid : m_rackMappings.keys())
    {
        auto it = std::find(bit, eit, cvmid);
        ISCORE_ASSERT(it != eit);
        (*it)->showRack(m_rackMappings.value(cvmid));
    }
}

void ClearConstraint::redo() const
{
    auto& constraint = m_path.find();

    // We make copies since the iterators might change.
    // TODO check if this is still valid wrt boost::multi_index
    auto processes = shallow_copy(constraint.processes);
    for(auto process : processes)
    {
        RemoveProcess(constraint, process->id());
    }

    auto rackes = shallow_copy(constraint.racks);
    for(const auto& rack : rackes)
    {
        constraint.racks.remove(rack->id());
    }
}

void ClearConstraint::serializeImpl(DataStreamInput& s) const
{
    s << m_path << m_serializedRackes << m_serializedProcesses << m_rackMappings;
}

void ClearConstraint::deserializeImpl(DataStreamOutput& s)
{
    s >> m_path >> m_serializedRackes >> m_serializedProcesses >> m_rackMappings;
}
}
}
