//
// Created by wx on 19-6-10.
//
#pragma  once
#include "Meter.h"
#include "EWMA.h"
#include "Histogram.h"
namespace mc {
class Timer :public Meter {

  //50
  //75
  //99
  EWMA p50;
  EWMA p75;
  Histogram histogram;
private:
  // AtomicLong count;
public:
  Timer() = default;
  Timer(Timer && src) noexcept = delete;    
  Timer(const Timer&) = delete;
  virtual ~Timer() {};


  Timer operator=(const Timer &) = delete;
  /**
   * @brief fetch and add
   * @param n to add
   * @return origin value before add.
   */
  int64_t getAndAdd(int n) {
    return count.fetch_add(1);
  }

  int64_t addAndGet(int n) {
    count.fetch_add(1);
    return count;
  }

  void inc(int n) {
    count.fetch_add(1);

  }
  void dec(int n) {
    //unsupported
  }
  virtual int64_t getCount() const {
    return count;
  }

  METRIC_TYPE getType() const override {
    return HIST;
  }
};
}