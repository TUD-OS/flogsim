#include <cmath>
#include <memory>

#include "configuration.hpp"
#include "globals.hpp"
#include "collective.hpp"
#include "task_queue.hpp"
#include "phase.hpp"
#include "fault_injector.hpp"

// Enable everybody
Collective::Collective()
  : Collective{{}, std::make_unique<NoFaults>()}
{
  reached_nodes.assign(reached_nodes.size(), true);
}

// Enable root selected
Collective::Collective(std::initializer_list<int> selected,
                       std::unique_ptr<FaultInjector> faults)
  : done_nodes(Globals::get().model().P),
    reached_nodes(Globals::get().model().P),
    faults(std::move(faults))
{
  for (auto i : selected) {
    reached_nodes[i] = true;
  }
}

void Collective::forward(const auto &t, TaskQueue &tq, const int node_id)
{
  Phase::Result res = phase->dispatch(t, tq, node_id);

  if (res == Phase::Result::DONE_PHASE || res == Phase::Result::DONE_COLL) {
    if (!done_nodes[node_id]) {
      tq.schedule(FinishTask::make_new(node_id));
      done_nodes[node_id] = true;
    }
  }
}

void Collective::accept(const InitTask &t, TaskQueue &tq)
{
  forward(t, tq, t.sender());
}

void Collective::accept(const TimerTask &t, TaskQueue &tq)
{
  forward(t, tq, t.sender());
}

void Collective::accept(const IdleTask &t, TaskQueue &tq)
{
  forward(t, tq, t.sender());
}

void Collective::accept(const SendEndTask &t, TaskQueue &tq)
{
  forward(t, tq, t.sender());
}

void Collective::accept(const SendStartTask&, TaskQueue&)
{
}

void Collective::accept(const RecvStartTask&, TaskQueue&)
{
}

void Collective::accept(const RecvEndTask& t, TaskQueue &tq)
{
  const int node_id = t.receiver();
  forward(t, tq, node_id);
}

void Collective::accept(const MsgTask&, TaskQueue&)
{
}

void Collective::accept(const FinishTask&, TaskQueue&)
{
}

void Collective::accept(const FailureTask&, TaskQueue&)
{
}

Timeline Collective::run(std::unique_ptr<Phase> &&_phase)
{
  phase = std::move(_phase);

  // Here we basically run it
  TaskQueue tq{faults.get()};
  Timeline timeline;

  // broadcast InitTask to all (initially) reached nodes
  for (size_t i = 0; i < reached_nodes.size(); i++) {
    tq.schedule(InitTask::make_new(Time(0), i));
  }

  tq.run(*this, timeline);

  return timeline;
}

CollectiveRegistry &CollectiveRegistry::get()
{
  static CollectiveRegistry reg;
  return reg;
}

void CollectiveRegistry::declare(CollectiveRegistry::create_fun_t &create_fun,
                                 const std::string_view &name)
{
  get().data[name] = create_fun;
}

std::unique_ptr<Phase> CollectiveRegistry::create(ReachedNodes &reached_nodes)
{
  const std::string &name = Globals::get().conf().collective;
  try {
    create_fun_t &create_fun = get().data.at(name);
    return create_fun(reached_nodes);
  } catch(const std::out_of_range &e) {
    throw std::invalid_argument("Collective does not exist:" +
                                name);
  }

}
