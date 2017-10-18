#include "model.hpp"

const LogP::Model &LogP::Model::get()
{
  static Model model{Time(1), Time(1), Time(2), 1023};

  return model;
}
