#include <metrics.hpp>

Metrics::value_t &Metrics::operator[](const std::string &name)
{
  if (map.find(name) == map.end()) {
    // Metric is not known yet. Create it.
    map[name] = 0;
  }
  return map.find(name)->second;
}

Metrics &Metrics::get()
{
  static Metrics metrics;
  return metrics;
}

void Metrics::reset_metrics()
{
  for (auto metric_ptr : Metrics::get().map) {
    metric_ptr.second = 0;
  }
}
