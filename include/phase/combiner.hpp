#pragma once

#include <memory>
#include <vector>

#include "phase.hpp"

class Combiner : public Phase
{
public:
  using PhasePtr = std::unique_ptr<Phase>;
  using PhaseVec = std::vector<PhasePtr>;

  struct Phases
  {
    std::vector<PhasePtr> phases;
    ReachedNodes &reached_nodes;

    template<typename T, typename ...Args>
    Phases &&add_phase(Args&&... args)
    {
      PhasePtr cur = std::make_unique<T>(reached_nodes, std::forward<Args>(args)...);
      phases.push_back(std::move(cur));
      return std::move(*this);
    }

    Phases &&add_phase(PhasePtr &&phase)
    {
      phases.push_back(std::move(phase));
      return std::move(*this);
    }

    Phases(ReachedNodes &reached_nodes)
      : reached_nodes(reached_nodes)
    {}

    Phases(const Phases &) = delete;
    Phases(Phases &&other)
      : phases(std::move(other.phases)),
        reached_nodes(other.reached_nodes)
    {}
  };

private:
  PhaseVec phases;
  std::vector<size_t> cur_phase;

  template <typename TASK>
  Result forward(const TASK &t, TaskQueue &tq, const int node_id);

public:
  Combiner(Phases &&phases);

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const IdleTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const TimerTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvStartTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvEndTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const SendStartTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const SendEndTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const FinishTask &t, TaskQueue &tq, int node_id) override;

  virtual Time deadline() const;
};
