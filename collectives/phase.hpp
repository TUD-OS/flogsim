#pragma once

#include <memory>
#include <vector>

#include "node_demux.hpp"
#include "globals.hpp"

class Time;
class TaskQueue;
class InitTask;
class IdleTask;
class TimerTask;
class RecvEndTask;
class NodeDemux;

/* Interface of a collective phase for a single node
 *
 * 'reached_nodes' holds information on which nodes have been reached. It is
 * both read and updated by each phase.
 */
class Phase
{
public:
  using ReachedVec = std::vector<bool>;
  using ReachedPtr = std::shared_ptr<ReachedVec>;

  // Helper class to grant access to 'reached_nodes' only
  class ReachAcc
  {
    static bool is_reached(const Phase &p, int node_id);
    static void mark_reached(Phase &p, int node_id);

    // restrictive handing out of priviledges
    friend void NodeDemux::accept(const RecvEndTask& t, TaskQueue& tq);
    friend void NodeDemux::accept(const InitTask &t, TaskQueue &tq);
  };

protected:
  const int num_nodes;      // number of participating nodes (P in LogP)
  ReachedPtr reached_nodes; // which nodes were reached already

  bool is_reached(int node_id);
  void mark_reached(int node_id);

public:
  enum class Result
  {
    ONGOING    = 1,
    DONE_PHASE = 2,
    DONE_COLL  = 3,
  };

  Phase(ReachedPtr reached_nodes);

  // process various possible events
  virtual Result dispatch(const InitTask &, TaskQueue &, int) = 0;
  virtual Result dispatch(const IdleTask &, TaskQueue &, int);
  virtual Result dispatch(const TimerTask &, TaskQueue &, int);
  virtual Result dispatch(const RecvEndTask &, TaskQueue &, int);

  // maximum time after which the phase will finish, i.e. report 'DONE_*'
  virtual Time deadline() const;
};
