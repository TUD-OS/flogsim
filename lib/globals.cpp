#include <stdexcept>

#include "globals.hpp"

static Globals &__get()
{
  static Globals globals;
  return globals;
}

const Globals &Globals::get()
{
  auto &globals = __get();
  if (!globals.initialized) {
    std::runtime_error("Initialize globals first");
  }
  return globals;
}

void Globals::set(const Globals &globals)
{
  __get() = globals;
}
