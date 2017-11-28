#pragma once

#include "integer.hpp"

struct Tag : public Integer<Tag>
{
  int tag;

  constexpr int get() const { return tag; }

  Tag() = default;
  constexpr explicit Tag(int tag)
    : tag(tag)
  {}
  Tag(const Tag &other) = default;
};
