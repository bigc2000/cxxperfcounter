//
// Created by wx on 18-8-11.
//

#pragma once


#include <atomic>
#include "../typedefines.h"
#include "Meter.h"

namespace mc {
class Counter : public Meter {
private:
  // AtomicLong count;
public:
  Counter() : Counter(0) {
  }

  /**
   * @brief with initial value
   * @param val
   */
  Counter(int64_t val) {
    count = val;
  }

  virtual  ~Counter() {
  }

  /**
   * @brief fetch and add
   * @param n to add
   * @return origin value before add.
   */
  int64_t getAndAdd(int n) {
    return count.fetch_add(n);
  }

  int64_t addAndGet(int n) {
    count.fetch_add(n);
    return count;
  }

  void inc() {
    count.fetch_add(1);
  }

  void dec() {
    count.fetch_add(-1);
  }

  void inc(int n) {
    count.fetch_add(n);
  }

  void dec(int n) {
    count.fetch_add(-n);
  }

  virtual int64_t getCount() const {
    return count;
  }

  METRIC_TYPE getType() const override {
    return COUNTER;
  }
};
}