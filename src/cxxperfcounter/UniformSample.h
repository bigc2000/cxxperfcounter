//
// Created by wx on 19-6-10.
// sliding window array
//

#pragma once
#include <utility>
#include <algorithm>
#include "Snapshot.h"
#include "../typedefines.h"

namespace mc {
static const int BITS_PER_LONG = 63;

class UniformSample {
  AtomicInteger first;
  AtomicInteger last;
  int stepPerTick; //like HZ
  std::vector<std::pair<INT64_T, double>> values;
public:

  UniformSample(int reservoirSize, int stepPerTick_) noexcept : first(0), last(0), stepPerTick(stepPerTick_), values(reservoirSize) {
  }

  UniformSample(const UniformSample &src) noexcept : first(src.first.load()), last(src.last.load()), values(src.values) {
  }

  UniformSample(UniformSample &&src) noexcept :first(src.first.load()), last(src.last.load()) {
    values = src.values;
  }
  UniformSample & operator =(const UniformSample &src) noexcept {
    first = src.first.load(std::memory_order::memory_order_relaxed);
    last = src.last.load(std::memory_order::memory_order_relaxed);
    values = src.values;
  }
  UniformSample & operator =(UniformSample &&src) noexcept {
    first = src.first.load(std::memory_order::memory_order_relaxed);
    last = src.last.load(std::memory_order::memory_order_relaxed);
    values = std::move(src.values);
  }

  ~UniformSample() noexcept = default;

  ///@brief no need to lock.
  int size() const {
    int N = values.size();
    int begin = first;
    int end = last;
    return (end + N - begin) % N;
  }

  ///@brief different to java, never use random to evict policy
  void update(INT64_T ts, double value) {
    int N = values.size();
    int cur = last;
    values[cur] = std::move(std::pair<INT64_T, double>(ts, value));
    bool conflict = false;
    do {
      int oldValue = last;
      int newValue = (oldValue + 1) % N;
      conflict = !last.compare_exchange_strong(oldValue, newValue, std::memory_order_release);
    } while (conflict);
  }
  void tick(int tickCount, INT64_T nowTime, INT64_T timeUnitPerSecond = 1) {
    int N = values.size();
    bool conflict = false;

    bool changed = false;
    int _5MinutesTick = (std::chrono::duration_cast<TIME_DURATION>(std::chrono::minutes(5))).count();
    int _10MinutesTick = _5MinutesTick * 2;
    do {
      int begin = first.load();
      int end = last;
      //    INT64_T diff = nowTime - std::duration_cast<TIME_DURATION>(std::)
          //5min-10£¬0.5
      int i = begin;
      int moveFirstTo = begin;
      for (i = begin; i != end; i++) {
        //begin to decay
        INT64_T diff = nowTime - values[i].first;
        if (diff > _10MinutesTick) {
          moveFirstTo++;
        }
        else break;
      }
      changed = first.compare_exchange_strong(begin, moveFirstTo, std::memory_order_release);
    } while (!changed);
  }

  Snapshot getSnapshot() {
    int N = values.size();
    int begin = first;
    int end = last;
    int count = (end + N - begin) % N;

    if (count == 0) {
      return Snapshot(std::vector < double >());
    }
    std::vector<double> cowVec(count);
    for (int i = 0; begin != end; i++) {
      cowVec[i] = values[begin].second;
      begin = (begin + 1) % N;
    }
    printf("snaphshot size=%d\n", count);
    return Snapshot(std::move(cowVec));
  }
};
}

