#include "tag.hpp"

std::ostream& operator<<(std::ostream &os, const Tag& t)
{
  switch (t) {
    case Tag::GENERIC:    os << "generic tag";    break;
    case Tag::TREE:       os << "tree tag";       break;
    case Tag::GOSSIP:     os << "gossip tag";     break;
    case Tag::RING_LEFT:  os << "left ring tag";  break;
    case Tag::RING_RIGHT: os << "right ring tag"; break;
  }
  return os;
}
