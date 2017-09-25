#pragma once
#include <boost/graph/adjacency_list.hpp>
#include <score/tools/std/HashMap.hpp>
#include <score_plugin_scenario_export.h>

namespace Scenario
{
class SynchronizationModel;
class IntervalModel;
class ScenarioInterface;

using GraphVertex = Scenario::SynchronizationModel*;
using GraphEdge = Scenario::IntervalModel*;

using Graph = boost::adjacency_list<
  boost::vecS,
  boost::vecS,
  boost::directedS,
  GraphVertex,
  GraphEdge>;

/**
 * @brief A directed graph of all the Synchronization%s in a ScenarioInterface.
 *
 * The vertices are the Synchronization%s, the edges are the IntervalModel%s.
 * The graph is built upon construction.
 *
 */
struct SCORE_PLUGIN_SCENARIO_EXPORT TimenodeGraph
{
  TimenodeGraph(const Scenario::ScenarioInterface& scenar);

  const Graph& graph() const
  { return m_graph; }
  const auto& edges() const
  { return m_edges; }
  const auto& vertices() const
  { return m_vertices; }

  //! Writes graphviz output on stdout
  void writeGraphviz();

private:
  Graph m_graph;

  score::hash_map<
      const Scenario::SynchronizationModel*,
      Graph::vertex_descriptor> m_vertices;
  score::hash_map<
      const Scenario::IntervalModel*,
      Graph::edge_descriptor> m_edges;
};

}
