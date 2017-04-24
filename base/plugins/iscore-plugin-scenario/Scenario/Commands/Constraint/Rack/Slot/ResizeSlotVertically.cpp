

#include "ResizeSlotVertically.hpp"
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/model/path/Path.hpp>
#include <iscore/model/path/PathSerialization.hpp>

namespace Scenario
{
namespace Command
{

ResizeSlotVertically::ResizeSlotVertically(
    const SlotIdentifier& slotPath,
    double newSize)
  : m_path{slotPath}, m_newSize{newSize}
{
  auto& slot = m_path.find();
  m_originalSize = slot.height;
}

ResizeSlotVertically::ResizeSlotVertically(
    SlotIdentifier&& slotPath, double newSize)
    : m_path{slotPath}, m_newSize{newSize}
{
  auto& slot = m_path.find();
  m_originalSize = slot.height;
}

void ResizeSlotVertically::undo() const
{
  auto& slot = m_path.constraint.find();
  slot.setSlotHeight(m_path, m_originalSize);
}

void ResizeSlotVertically::redo() const
{
  auto& slot = m_path.constraint.find();
  slot.setSlotHeight(m_path, m_newSize);
}

void ResizeSlotVertically::serializeImpl(DataStreamInput& s) const
{
  s << m_path << m_originalSize << m_newSize;
}

// Would be better in a ctor ?
void ResizeSlotVertically::deserializeImpl(DataStreamOutput& s)
{
  s >> m_path >> m_originalSize >> m_newSize;
}
}
}
