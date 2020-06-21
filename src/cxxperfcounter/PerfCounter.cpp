//
// Created by wx on 18-8-11.
//

#include "PerfCounter.h"
#include "MetricRegistry.h"

using namespace mc;

INT64_T PerfCounter::count(const std::string &name, int cnt) {
  CounterPtr counterPtr = MetricRegistry::getInstance()->counter(name);
  counterPtr->mark(cnt);
  return counterPtr->getCount();
}
void PerfCounter::count(const std::string &name, int cnt, double timecost) {
  CounterPtr counterPtr = MetricRegistry::getInstance()->counter(name);
  counterPtr->mark(cnt);
  MetricRegistry::getInstance()->histogram(name)->update(cnt,timecost/cnt);
}

void PerfCounter::countDuration(const std::string &name, double timecost) {
  MetricRegistry::getInstance()->histogram(name)->update(timecost);
}