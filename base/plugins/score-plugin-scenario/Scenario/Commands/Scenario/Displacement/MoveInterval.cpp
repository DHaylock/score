// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <Scenario/Document/Interval/IntervalModel.hpp>
#include <Scenario/Process/Algorithms/VerticalMovePolicy.hpp>
#include <Scenario/Process/ScenarioModel.hpp>

#include <algorithm>

#include "MoveInterval.hpp"
#include <Process/TimeValue.hpp>
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/model/EntityMap.hpp>
#include <score/model/path/PathSerialization.hpp>

namespace Scenario
{
namespace Command
{
MoveInterval::MoveInterval(
    const Scenario::ProcessModel& scenar,
    Id<IntervalModel> id,
    double height)
    : m_path{scenar}, m_interval{id}, m_newHeight{height}
{
  auto& cst = scenar.intervals.at(m_interval);
  m_oldHeight = cst.heightPercentage();

  auto list = selectedElements(scenar.intervals);

  for (auto& elt : list)
  {
    m_selectedIntervals.append({elt->id(), elt->heightPercentage()});
  }

  if (m_selectedIntervals.empty())
    m_selectedIntervals.append({m_interval, m_oldHeight});
}

void MoveInterval::undo(const score::DocumentContext& ctx) const
{
  auto& scenar = m_path.find(ctx);
  for (const auto& cstr : m_selectedIntervals)
  {
    updateIntervalVerticalPos(cstr.second, cstr.first, scenar);
  }
}

void MoveInterval::redo(const score::DocumentContext& ctx) const
{
  auto& scenar = m_path.find(ctx);
  for (const auto& cstr : m_selectedIntervals)
  {
    updateIntervalVerticalPos(
        cstr.second + m_newHeight - m_oldHeight, cstr.first, scenar);
  }
}

void MoveInterval::serializeImpl(DataStreamInput& s) const
{
  s << m_path << m_interval << m_oldHeight << m_newHeight
    << m_selectedIntervals;
}

void MoveInterval::deserializeImpl(DataStreamOutput& s)
{
  s >> m_path >> m_interval >> m_oldHeight >> m_newHeight
      >> m_selectedIntervals;
}
}
}
