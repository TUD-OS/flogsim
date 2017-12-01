#include "collective.hpp"
#include "task_queue.hpp"
#include "globals.hpp"

class BinaryBroadcast : public Collective
{
  int nodes;
  void post_sends(int sender, TaskQueue &tq)
  {
    for (int i = 1; i <= 2; i++) {
      int recv = 2 * sender + i;
      if (recv < nodes) {
        tq.schedule(SendStartTask::make_new(Tag(0), tq.now(), sender, recv));
      }
    }
    tq.schedule(FinishTask::make_new(sender));
  }
public:
  BinaryBroadcast()
    : Collective(),
      nodes(Globals::get().model().P)
  {
  }

  virtual void accept(const RecvStartTask& task, TaskQueue& tq)
  {
    post_sends(task.receiver(), tq);
  }

  virtual void accept(const InitTask &, TaskQueue &tq)
  {
    int root = 0;
    post_sends(root, tq);
  }

public:
  static constexpr std::string_view name = "binary_bcast";
};
