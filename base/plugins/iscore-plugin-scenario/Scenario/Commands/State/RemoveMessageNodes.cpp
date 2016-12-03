#include <Scenario/Document/State/ItemModel/MessageItemModelAlgorithms.hpp>
#include <algorithm>

#include "RemoveMessageNodes.hpp"
#include <Scenario/Document/State/ItemModel/MessageItemModel.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/tools/ModelPath.hpp>
#include <iscore/tools/ModelPathSerialization.hpp>
#include <iscore/tools/TreeNode.hpp>
namespace Scenario
{
namespace Command
{

RemoveMessageNodes::RemoveMessageNodes(
    Path<StateModel>&& device_tree,
    const QList<const Process::MessageNode*>& nodes)
    : m_path{device_tree}
{
  auto model = m_path.try_find();
  if (model)
  {
    m_oldState = model->messages().rootNode();
  }

  m_newState = m_oldState;
  for (const auto& node : nodes)
  {
    updateTreeWithRemovedNode(m_newState, address(*node).address);
  }
}

void RemoveMessageNodes::undo() const
{
  auto& model = m_path.find().messages();
  model = m_oldState;
}

void RemoveMessageNodes::redo() const
{
  auto& model = m_path.find().messages();
  model = m_newState;
}

void RemoveMessageNodes::serializeImpl(DataStreamInput& d) const
{
  d << m_path << m_oldState << m_newState;
}

void RemoveMessageNodes::deserializeImpl(DataStreamOutput& d)
{
  d >> m_path >> m_oldState >> m_newState;
}
}
}
