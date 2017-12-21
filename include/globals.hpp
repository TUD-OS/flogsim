#pragma once

#include "configuration.hpp"
#include "model.hpp"
#include "entropy.hpp"

class Model;

class Globals
{
  bool initialized;

  const Configuration *_conf;
  const Model *_model;
  Entropy *_entropy;
public:
  const Configuration &conf() const
  {
    return *_conf;
  }

  const Model &model() const
  {
    return *_model;
  }

  Entropy &entropy() const
  {
    return *_entropy;
  }

  Globals()
    : initialized(false), _conf(nullptr), _model(nullptr), _entropy(nullptr)
  {}
  Globals(const Configuration *conf, const Model *model, Entropy *entropy)
    : initialized(true), _conf(conf), _model(model), _entropy(entropy)
  {}

  static const Globals &get();
  static void set(const Globals &globals);
};
