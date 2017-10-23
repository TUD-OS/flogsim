#include <cmath>

#include "collective.hpp"
#include "task_queue.hpp"

class BinaryBroadcast : public Collective
{
  int nodes;
  void post_sends(int sender, TaskQueue &tq)
  {
    for (int i = 1; i <= 2; i++) {
      int recv = 2 * sender + i;
      if (recv < nodes) {
        tq.schedule(std::make_shared<LogP::SendTask>(tq.now(), sender, recv));
      }
    }
    tq.schedule(std::make_shared<LogP::FinishTask>(sender));
  }
public:
  BinaryBroadcast()
    : nodes(Configuration::get().P)
  {
  }

  virtual void accept(const LogP::RecvTask& task, TaskQueue& tq)
  {
    post_sends(task.receiver(), tq);
  }

  void populate(TaskQueue &tq) override
  {
    int root = 0;
    post_sends(root, tq);
  }
};

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
        tq.schedule(std::make_shared<LogP::SendTask>(tq.now(), sender, recv));
      }
    }

    for (int i = 1; i <= k - 1; i++) {
      int recv = (sender + nodes - i) % nodes;
      tq.schedule(std::make_shared<LogP::SendTask>(tq.now(), sender, recv));
    }

    tq.schedule(std::make_shared<LogP::FinishTask>(sender));
    done[sender] = true;
  }

public:
  CorrectedTreeBroadcast()
    : k(Configuration::get().k),
      nodes(Configuration::get().P),
      done(nodes)
  {}

  virtual void accept(const LogP::RecvTask& task, TaskQueue& tq)
  {
    post_sends(task.receiver(), tq);
  }

  void populate(TaskQueue &tq) override
  {
    int root = 0;
    post_sends(root, tq);
  }
};

std::unique_ptr<Collective> Collective::create()
{
  auto &conf = Configuration::get();

  if (conf.collective == "binary_bcast") {
    return std::make_unique<BinaryBroadcast>();
  } else if (conf.collective == "correctedtree_bcast") {
    return std::make_unique<CorrectedTreeBroadcast>();
  } else {
    throw std::invalid_argument("Collective does not exist:" +
                                conf.fault_injector);
  }
}
