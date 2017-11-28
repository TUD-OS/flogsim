#pragma once

#include <algorithm>

#include <cmath>

#include "collective.hpp"
#include "task_queue.hpp"

#include "ctb_node.hpp"

template<bool send_over_root>
class CorrectedTreeBroadcast : public Collective
{
protected:
  int k;
  int nodes;

  typedef CTBNode<CorrectedTreeBroadcast, send_over_root> node_t;
  friend node_t;

  std::vector<node_t> nodeset;

  virtual Time correction_phase_start(int k)
  {
    return Time(0);
  }

public:
  CorrectedTreeBroadcast()
    : Collective(),
      k(Globals::get().conf().k),
      nodes(Globals::get().model().P),
      nodeset(nodes)
  {
    for (int i = 0; i < nodes; i++) {
      nodeset[i].id = i;
      nodeset[i].right_min_recv = nodes;
      nodeset[i].left_min_recv = nodes;
    }
  }

  virtual void accept(const IdleTask &task, TaskQueue &tq)
  {
    node_t &node = nodeset[task.sender()];

    if (!node.pending_sends) {
      node.post_next_message(*this, tq);
    }
  }

  virtual void accept(const SendEndTask &task, TaskQueue &tq)
  {
    node_t &node = nodeset[task.sender()];

    node.pending_sends--;
    node.accept_send_end(*this, task);
    tq.schedule(IdleTask::make_new(node.id));
  }

  virtual void accept(const RecvEndTask& task, TaskQueue& tq)
  {
    node_t &node = nodeset[task.receiver()];

    node.accept_receive(*this, task);
    tq.schedule(IdleTask::make_new(node.id));
  }

  virtual void accept(const InitTask &task, TaskQueue &tq)
  {
    node_t &root = nodeset[0];
    root.tree_recv = true;
    root.post_next_message(*this, tq);
  }
};
