#pragma once

#include <vector>

#include "time.hpp"

enum class NodeOrder {
  INORDER,
  INTERLEAVED
};

class Topology {
  struct Node {
    std::vector<int> children;
    std::vector<int> parents;
  };

  std::vector<Node> nodes;

  bool direction_down = true;
  NodeOrder order;
  protected:
    void add_edge(int sender, int receiver) {
      nodes[sender].children.push_back(receiver);
      nodes[receiver].parents.push_back(sender);
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

    const std::vector<int> &peers(int sender) const {
      return direction_down ? nodes[sender].children : nodes[sender].parents;
    }

    virtual Time deadline() const = 0;
};