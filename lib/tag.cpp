#include "tag.hpp"

std::ostream& operator<<(std::ostream &os, const Tag& t)
{
  switch (t) {
    case Tag::INTERNAL:   os << "internal";   break;
    case Tag::TREE:       os << "tree";       break;
    case Tag::GOSSIP:     os << "gossip";     break;
    case Tag::RING_LEFT:  os << "left_ring";  break;
    case Tag::RING_RIGHT: os << "right_ring"; break;
    case Tag::EXCLUSIVE:  os << "exclusive";  break;
  }
  return os;
}
