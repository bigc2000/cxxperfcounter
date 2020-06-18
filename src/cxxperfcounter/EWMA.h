#pragma once
/**
 * @brief copied from metrics.dropwizard.io
 */
#include "../typedefines.h"
#include <chrono>
#include <ctime>
#include <algorithm>
#include "Clock.h"

namespace mc {

class EWMA {
  constexpr static const double M1_ALPHA = 0.07995558537067671;// 1.0D - Math.exp(-0.08333333333333333D);
  constexpr static const double M5_ALPHA = 0.01652854617838251;//1.0D - Math.exp(-0.016666666666666666D);
  constexpr static const double M15_ALPHA = 0.00554015199509772;//1.0D - Math.exp(-0.005555555555555555D);
  AtomicBoolean initialized;
  AtomicDouble rate;
  AtomicLong uncounted;
  double alpha;
  double interval;
public:

  EWMA() : initialized(false), rate(0), uncounted(0), alpha(0.6), interval(std::chrono::seconds(5L).count()) {};

  EWMA(const EWMA &ewma) : alpha(ewma.alpha), interval(ewma.interval) {
    this->initialized.store(ewma.initialized);
    this->rate.store(ewma.rate);
  }

  EWMA(double alpha, std::chrono::seconds intervalUnit) : initialized(false), rate(0), uncounted(0) {
    this->interval = intervalUnit.count();
    this->alpha = alpha;
  }

  EWMA& operator=(const EWMA &ewma) {
    alpha = ewma.alpha;
    interval = ewma.interval;
    this->initialized.store(ewma.initialized);
    this->rate.store(ewma.rate);
    return *this;
  }

public:

  static EWMA* oneMinuteEWMA() {
    return new EWMA(M1_ALPHA, std::chrono::seconds(5L));

  }


  static EWMA* fiveMinuteEWMA() {
    return new EWMA(M5_ALPHA, std::chrono::seconds(5L));

  }


  static EWMA* fifteenMinuteEWMA() {
    return new EWMA(M15_ALPHA, std::chrono::seconds(5L));
  }


  void update(int64_t n) {
    uncounted.fetch_add(n, std::memory_order_relaxed);
  }


  void tick() {
    //这两句不能保证原子，怎么合起来比较好？
    int64_t count = uncounted.load();
    uncounted.store(0);
    double instantRate = count / interval;
    bool changed = false;
    if (initialized) {
      double old = 0.0;
      do {
        old = rate.load();
        changed = rate.compare_exchange_strong(old, rate + alpha * (instantRate - rate));
      } while (!changed);
    } else {
      rate = instantRate;
      initialized = true;
    }
  }

  double getRate() {
    return rate;
  }
};
}