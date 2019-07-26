#pragma once

#include <memory>
#include <functional>
#include <map>
#include <string>
#include <type_traits>

#include "reached_nodes.hpp"
#include "timeline.hpp"
#include "phase.hpp"
#include "topology/topology.hpp"

class SendStartTask;
class SendEndTask;
class RecvStartTask;
class RecvEndTask;
class MsgTask;
class IdleTask;
class FinishTask;
class TimerTask;
class InitTask;
class FailureTask;

class TaskQueue;
class FaultInjector;

/* Act as a collective and forward relevant Tasks to individual nodes */
//
// This class is responsible to set up reach_nodes. It either accepts
// explicit list of reached nodes as an initializer list (for example
// only root: {0}) or it enables nobody.
class Collective
{
protected:
  const Topology topology;
private:
  void forward(const auto &t, TaskQueue &tq, const int node_id);

  std::unique_ptr<Phase> phase;
  ReachedNodes done_nodes;
  // Vector of nodes marked as coloured. Collective gathers statistics
  // when every node becomes coloured.
  ReachedNodes coloured_nodes;
  Time coloured_time = Time(0);
public:
  ReachedNodes reached_nodes;
  FaultInjector *faults;

  virtual void accept(const InitTask &t, TaskQueue &tq);
  virtual void accept(const TimerTask &t, TaskQueue &tq);
  virtual void accept(const IdleTask &t, TaskQueue &tq);

  virtual void accept(const SendStartTask &t, TaskQueue &tq);
  virtual void accept(const SendEndTask &t, TaskQueue &tq);

  virtual void accept(const RecvStartTask &t, TaskQueue &tq);
  virtual void accept(const RecvEndTask &t, TaskQueue &tq);

  virtual void accept(const MsgTask &t, TaskQueue &tq);
  virtual void accept(const FinishTask &t, TaskQueue &tq);
  virtual void accept(const FailureTask &t, TaskQueue &tq);

  void run(Timeline &, std::unique_ptr<Phase> &&);

  Collective(FaultInjector *faults,
             Topology _topology);
  Collective(std::initializer_list<int> selected,
             FaultInjector *faults,
             Topology _topology);
  //  Factory method, which creates collectives based on
  // configuration.
  static std::unique_ptr<Collective> create();
};
