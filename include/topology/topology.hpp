#pragma once

#include <vector>

#include "time.hpp"
#include "rank.hpp"

enum class NodeOrder {
  INORDER,
  INTERLEAVED
};

class Topology {
  struct Node {
    std::vector<Rank> children;
    std::vector<Rank> parents;
  };

  std::vector<Node> nodes;

  bool direction_down = true;
  NodeOrder order;
  protected:
    void add_edge(Rank sender, Rank receiver) {
      nodes[sender.get()].children.push_back(receiver);
      nodes[receiver.get()].parents.push_back(sender);
    }

    int num_nodes() const { return nodes.size(); }
  public:
    Topology(int num_nodes, NodeOrder order) : nodes(num_nodes), order(order) {}

    auto &lookup_down() {
      direction_down = true;
      return *this;
    }

    auto &lookup_up() {
      direction_down = false;
      return *this;
    }

    const std::vector<Rank> &receivers(Rank sender) const {
      return direction_down ? nodes[sender.get()].children : nodes[sender.get()].parents;
    }

    const std::vector<Rank> &senders(Rank receiver) const {
      return direction_down ? nodes[receiver.get()].parents : nodes[receiver.get()].children;
    }

    const std::vector<Rank> leaves() const {
      std::vector<Rank> leaves;
      for(int i = 0; i < num_nodes(); i++) {
        if(nodes[i].children.size() == 0) {
          leaves.push_back(Rank(i));
        }
      }
      return leaves;
    }

    Time deadline() const;
};
