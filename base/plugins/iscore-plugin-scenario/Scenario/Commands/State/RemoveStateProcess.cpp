#include "RemoveStateProcess.hpp"
#include <Process/StateProcess.hpp>
#include <Process/StateProcessFactory.hpp>
#include <Process/StateProcessFactoryList.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <iscore/model/path/PathSerialization.hpp>
#include <iscore/tools/IdentifierGeneration.hpp>
#include <iscore/application/ApplicationContext.hpp>
namespace Scenario
{
namespace Command
{

RemoveStateProcess::RemoveStateProcess(
    Path<StateModel>&& statePath, Id<Process::StateProcess> processId)
    : m_path{std::move(statePath)}, m_processId{std::move(processId)}
{
  auto& state = m_path.find();
  auto& p = state.stateProcesses.at(m_processId);
  m_processUuid = p.concreteKey();
}

void RemoveStateProcess::undo() const
{
  auto& state = m_path.find();
  // Create process model
  auto proc = context.interfaces<Process::StateProcessList>()
                  .get(m_processUuid)
                  ->make(m_processId, &state);

  state.stateProcesses.add(proc);
}

void RemoveStateProcess::redo() const
{
  auto& state = m_path.find();
  state.stateProcesses.remove(m_processId);
}

void RemoveStateProcess::serializeImpl(DataStreamInput& s) const
{
  s << m_path << m_processUuid << m_processId;
}

void RemoveStateProcess::deserializeImpl(DataStreamOutput& s)
{
  s >> m_path >> m_processUuid >> m_processId;
}
}
}
