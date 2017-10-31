#pragma once
/*
 * Extension of Configuration structure to populate data from command
 * line. Reason to implement it as separate class is that
 * ConfigurationArgs depends on Boost. I want to make dependency on
 * Boost as explicit as possible.
 */
#include <cinttypes>

#include "configuration.hpp"

struct ConfigurationArgs : public Configuration
{
  ConfigurationArgs(int argc, char *argv[]);
};
