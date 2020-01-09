//
// https://github.com/flandr/ccmetrics
//

#pragma once

#include <map>
#include <vector>
#include <functional>
#include <algorithm>
#include <mutex>
#include "Gauge.h"
#include "Counter.h"
#include "Histogram.h"
#include "EventTimer.h"

namespace mc {

typedef Gauge *GaugePtr;
typedef Counter *CounterPtr;
typedef Histogram *HistogramPtr;
typedef Meter *MeterPtr;
typedef Metric *MetricPtr;

class MetricRegistry {
protected:
  mutable std::mutex m_mutex;
  std::map<std::string, CounterPtr> counterMap;
  std::map<std::string, HistogramPtr> histMap;

protected:
  MetricRegistry() {}

  ~MetricRegistry() {
    //if not smarter pointer , release manual.
  }

  CounterPtr newCounter() {
    return new Counter;
  }

  HistogramPtr newHistogram() {
    return new Histogram;
  }

public:
  static MetricRegistry *getInstance() {
    static MetricRegistry registry;
    return &registry;
  }

  std::vector<MetricPtr> getMeters() const {
    std::vector<MetricPtr> vec;
    for (auto &it: counterMap) {
      vec.push_back((MetricPtr) it.second);
    }
    for (auto &it : histMap) {
      vec.push_back((MetricPtr) it.second);
    }
    return vec;//rvo
  }

  CounterPtr counter(const std::string &metricName) {
    std::lock_guard<decltype(m_mutex)> guard(m_mutex);
    std::pair<std::map<std::string, CounterPtr>::iterator, bool> it = counterMap.insert(
        std::make_pair<std::string, CounterPtr>(std::string(metricName), (CounterPtr) nullptr));
    if (it.second) {
      //not exists,first time to add.
      it.first->second = newCounter();
    } else {
      //exist
    }
    CounterPtr ptr = it.first->second;
    ptr->name(metricName);
    return ptr;
  }

  HistogramPtr histogram(const std::string &metricName) {
    std::lock_guard<decltype(m_mutex)> guard(m_mutex);
    std::pair<std::map<std::string, HistogramPtr>::iterator, bool> it = histMap.insert(
        std::make_pair<std::string, HistogramPtr>(std::string(metricName), (HistogramPtr) nullptr));
    if (it.second) {
      //not exists,first time to add.
      it.first->second = newHistogram();
    } else {
      //exist
    }
    auto &ptr = it.first->second;
    ptr->name(metricName);
    return ptr;
  }
};
}

