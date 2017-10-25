#pragma once

#include "integer.hpp"

struct Tag : public Integer<Tag>
{
  int tag;

  int get() const { return tag; }

  Tag() = default;
  explicit Tag(int tag)
    : tag(tag)
  {}
  Tag(const Tag &other) = default;
};
