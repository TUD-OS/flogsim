#include "sequence.hpp"

Sequence Sequence::next()
{
  static int next_id = 0;
  return Sequence(next_id++);
}
