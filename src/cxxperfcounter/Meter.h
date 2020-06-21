#pragma once

#include "Metered.h"
#include "EWMA.h"
#include "Clock.h"
/**
 * A meter metric which measures mean throughput and one-, five-, and fifteen-minute
 * exponentially-weighted moving average throughput.
 *
 * @see EWMA
 */
namespace mc {
class Meter : public Metered {
public:

protected:
  std::unique_ptr<EWMA> m1Rate;
  std::unique_ptr<EWMA> m5Rate;
  std::unique_ptr<EWMA> m15Rate;

  AtomicLong count;
  INT64_T startTime;
  AtomicLong lastTick;
  CLOCK clock;//时钟
public:
  /**
   * Creates a new {@link Meter}.
   *
   * @param clock      the clock to use for the meter ticks
   */
  Meter() noexcept {
    count = 0;
    startTime = GetNowTimeCount();
    lastTick = startTime;
    m1Rate = std::unique_ptr<EWMA>(EWMA::oneMinuteEWMA());
    m5Rate = std::unique_ptr<EWMA>(EWMA::fiveMinuteEWMA());
    m15Rate = std::unique_ptr<EWMA>(EWMA::fifteenMinuteEWMA());
  }

  virtual ~Meter() {}


  /**
   * Creates a new {@link Meter}.
   */
public:


  /**
   * Mark the occurrence of an event.
   */
  void mark() {
    mark(1);
  }

  /**
   * Mark the occurrence of a given number of events.
   *
   * @param n the number of events
   */
  void mark(int n) {
    tickIfNecessary();
    count.fetch_add(n, std::memory_order_relaxed);
    m1Rate->update(n);
    m5Rate->update(n);
    m15Rate->update(n);
  }

  void tickIfNecessary() {
    INT64_T oldTick = lastTick;
    INT64_T newTick = GetNowTimeCount();
    INT64_T age = newTick - oldTick;
    if (age > TICK_INTERVAL) {
      INT64_T newIntervalStartTick = newTick - age % TICK_INTERVAL;
      if (lastTick.compare_exchange_strong(oldTick, newIntervalStartTick)) {
        INT64_T requiredTicks = age / TICK_INTERVAL;
        for (long i = 0; i < requiredTicks; i++) {
          m1Rate->tick();
          m5Rate->tick();
          m15Rate->tick();
        }
      }
    }
  }

  /**
   * @brief 只会累加计数
   * @return
   */
  int64_t getCount() const override {
    return count;
  }

  double getOneMinuteRate() override {
    tickIfNecessary();
    return m1Rate->getRate();
  }

  double getFifteenMinuteRate() override {
    tickIfNecessary();
    return m15Rate->getRate();
  }

  double getFiveMinuteRate() override {
    tickIfNecessary();
    return m5Rate->getRate();
  }


  double getMeanRate() override {
    if (getCount() == 0) {
      return 0.0;
    }
    else {
      double elapsed = GetNowTimeCount() - startTime;
      return getCount() / elapsed;
    }
  }
};
}