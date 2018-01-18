#pragma once

#include <memory>
#include <vector>

#include "reached_nodes.hpp"
#include "globals.hpp"

class Time;
class TaskQueue;
class InitTask;
class IdleTask;
class TimerTask;
class RecvEndTask;
class SendEndTask;
class FinishTask;

/* Interface of a collective phase for a single node
 *
 * 'reached_nodes' holds information on which nodes have been reached. It is
 * both read and updated by each phase.
 */
class Phase
{
public:

protected:
  ReachedNodes &reached_nodes; // which nodes were reached already

  int num_nodes() const { return reached_nodes.size(); }
public:
  enum class Result
  {
    ONGOING    = 1,
    DONE_PHASE = 2,
    DONE_COLL  = 3,
  };

  Phase(ReachedNodes &reached_nodes)
    : reached_nodes(reached_nodes)
  {}
  virtual ~Phase() {}

  // process various possible events
  virtual Result dispatch(const InitTask &, TaskQueue &, int) = 0;
  virtual Result dispatch(const IdleTask &, TaskQueue &, int);
  virtual Result dispatch(const TimerTask &, TaskQueue &, int);
  virtual Result dispatch(const RecvEndTask &, TaskQueue &, int);
  virtual Result dispatch(const SendEndTask &, TaskQueue &, int);
  virtual Result dispatch(const FinishTask &, TaskQueue &, int);

  // maximum time after which the phase will finish, i.e. report 'DONE_*'
  virtual Time deadline() const;
};
