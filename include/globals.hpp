#pragma once

#include "configuration.hpp"
#include "model.hpp"

class Model;

class Globals
{
  bool initialized;

  const Configuration *_conf;
  const Model *_model;
public:
  const Configuration &conf() const
  {
    return *_conf;
  }

  const Model &model() const
  {
    return *_model;
  }

  Globals()
    : initialized(false), _conf(nullptr), _model(nullptr)
  {}
  Globals(const Configuration *conf, const Model *model)
    : initialized(true), _conf(conf), _model(model)
  {}

  static const Globals &get();
  static void set(const Globals &globals);
};
