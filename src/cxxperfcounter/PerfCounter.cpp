//
// Created by wx on 18-8-11.
//

#include "PerfCounter.h"
#include "MetricRegistry.h"

using namespace mc;

int64_t PerfCounter::count(const std::string &name, int cnt) {
  CounterPtr counterPtr = MetricRegistry::getInstance()->counter(name);
  counterPtr->mark(cnt);
  return counterPtr->getCount();
}

void PerfCounter::countDuration(const std::string &name, int cnt, int timecost) {
  for (int i = 0; i < cnt; i++) {
    MetricRegistry::getInstance()->histogram(name)->update(timecost);
  }
}