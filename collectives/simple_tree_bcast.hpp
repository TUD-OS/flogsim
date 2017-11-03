#include "collective.hpp"
#include "task_queue.hpp"
#include "globals.hpp"

class SimpleTreeBroadcast : public Collective
{
  int const nodes, k;

  void post_sends(int sender, TaskQueue &tq) const
  {
    for (int cc = 1; cc <= k; ++cc) {
      int const recv = k * sender + cc;
      if (recv < nodes) {
        tq.schedule(SendStartTask::make_new(Tag(0), tq.now(), sender, recv));
      }
    }
    tq.schedule(FinishTask::make_new(sender));
  }
public:
  SimpleTreeBroadcast()
    : Collective(),
    nodes(Globals::get().model().P), k(Globals::get().conf().k)
  {
  }

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
