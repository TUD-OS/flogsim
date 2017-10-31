#include <cmath>

#include "collective.hpp"
#include "task_queue.hpp"

class CorrectedTreeBroadcast : public Collective
{
  int k;
  int nodes;
  std::vector<bool> done;

  void post_sends(int sender, TaskQueue &tq)
  {
    if (done[sender]) {
      return;
    }

    int lvl = int(std::log(sender + 1) / std::log(k));
    for (int i = 1; i <= k; i++) {
      int recv = sender + i * std::pow(k, lvl);
      if (recv < nodes) {
        tq.schedule(SendStartTask::make_new(Tag(0), tq.now(), sender, recv));
      }
    }

    for (int i = 1; i <= k - 1; i++) {
      int recv = (sender + nodes - i) % nodes;
      tq.schedule(SendStartTask::make_new(Tag(0), tq.now(), sender, recv));
    }

    tq.schedule(FinishTask::make_new(tq.now(), sender));
    done[sender] = true;
  }

public:
  CorrectedTreeBroadcast(const Configuration &conf)
    : Collective(conf),
      k(conf.k),
      nodes(conf.P),
      done(nodes)
  {}

  virtual void accept(const RecvStartTask& task, TaskQueue& tq)
  {
    post_sends(task.receiver(), tq);
  }

  void populate(TaskQueue &tq) override
  {
    int root = 0;
    post_sends(root, tq);
  }
};
