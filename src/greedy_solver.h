#ifndef CYCLUS_GREEDY_SOLVER_H_
#define CYCLUS_GREEDY_SOLVER_H_

#include "exchange_solver.h"
#include "exchange_graph.h"

namespace cyclus {
  
/// @brief The GreedySolver provides the implementation for a "greedy" solution
/// to a resource exchange graph.
///
/// Given an ExchangeGraph, the greedy solver will march through each RequestSet
/// in the graph, matching request nodes "greedily" with supply nodes. Each
/// request node will attempt to be supplied by supplier arcs as long as those
/// supplier arcs have some excess capacity. The algorithm terminates when one
/// of the following conditions is met:
///   1) All RequestSets are satisfied
///   2) All SupplySets are at capacity
class GreedySolver: public ExchangeSolver {
 public:
  explicit GreedySolver(ExchangeGraph& g);
  virtual void Solve();
  void __GreedilySatisfySet(RequestSet::Ptr prs);
};

} // namespace cyclus

#endif // ifndef CYCLUS_GREEDY_SOLVER_H_