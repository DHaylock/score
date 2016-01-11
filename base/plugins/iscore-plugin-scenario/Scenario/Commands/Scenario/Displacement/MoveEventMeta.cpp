#include <Scenario/Commands/Scenario/Displacement/MoveEventList.hpp>

#include <QByteArray>
#include <algorithm>

#include "MoveEventFactoryInterface.hpp"
#include "MoveEventMeta.hpp"
#include <Scenario/Commands/Scenario/Displacement/SerializableMoveEvent.hpp>
#include <iscore/application/ApplicationContext.hpp>
#include <iscore/plugins/customfactory/StringFactoryKey.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>

namespace Scenario {
}  // namespace Scenario
#include <iscore/tools/SettableIdentifier.hpp>


namespace Scenario
{
namespace Command
{
class EventModel;
class ScenarioModel;
MoveEventMeta::MoveEventMeta(
        Path<Scenario::ScenarioModel>&& scenarioPath,
        const Id<EventModel>& eventId,
        const TimeValue& newDate,
        ExpandMode mode)
    :SerializableMoveEvent{},
     m_moveEventImplementation(
         context.components.factory<MoveEventList>()
         .get(MoveEventFactoryInterface::Strategy::MOVING)
         ->make(std::move(scenarioPath), eventId, newDate, mode))
{
}

void MoveEventMeta::undo() const
{
    m_moveEventImplementation->undo();
}

void MoveEventMeta::redo() const
{
    m_moveEventImplementation->redo();
}

const Path<Scenario::ScenarioModel>&MoveEventMeta::path() const
{
    return m_moveEventImplementation->path();
}

void MoveEventMeta::serializeImpl(DataStreamInput& qDataStream) const
{
    qDataStream << m_moveEventImplementation->serialize();
}

void MoveEventMeta::deserializeImpl(DataStreamOutput& qDataStream)
{
    QByteArray cmdData;

    qDataStream >> cmdData;

    m_moveEventImplementation =
            context.components.factory<MoveEventList>()
            .get(MoveEventFactoryInterface::Strategy::MOVING)->make();

    m_moveEventImplementation->deserialize(cmdData);
}

void MoveEventMeta::update(const Path<Scenario::ScenarioModel>& scenarioPath,
                           const Id<EventModel>& eventId,
                           const TimeValue& newDate,
                           ExpandMode mode)
{
    m_moveEventImplementation->update(scenarioPath, eventId, newDate, mode);
}
}
}
