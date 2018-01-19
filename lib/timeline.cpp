#include "timeline.hpp"

void Timeline::store_metrics()
{
  auto &metrics = Globals::get().metrics();

  metrics["TotalRuntime"] = total_time.get();
}
