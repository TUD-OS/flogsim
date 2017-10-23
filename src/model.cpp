#include "model.hpp"
#include "configuration.hpp"

const LogP::Model &LogP::Model::get()
{
  auto &conf = Configuration::get();
  static Model model{Time(conf.L),
                     Time(conf.o),
                     Time(conf.g),
                     conf.P};

  return model;
}
