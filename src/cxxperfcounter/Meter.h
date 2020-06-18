#pragma once


#include <chrono>
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
  constexpr const static uint64_t TICK_INTERVAL =
    std::chrono::duration_cast<TIME_DURATION>(std::chrono::seconds(5)).count();
protected:
  std::unique_ptr<EWMA> m1Rate;
  std::unique_ptr<EWMA> m5Rate;
  std::unique_ptr<EWMA> m15Rate;

  AtomicUInteger count;
  uint64_t startTime;
  AtomicULong lastTick;
  CLOCK clock;//时钟
public:
  Meter() noexcept : Meter(CLOCK()) {
  }

  /**
   * Creates a new {@link Meter}.
   *
   * @param clock      the clock to use for the meter ticks
   */
  Meter(CLOCK clock)noexcept {
    count = 0;
    this->clock = clock;
    startTime = clock.now().time_since_epoch().count();
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
  void mark(uint64_t n) {
    tickIfNecessary();
    count.fetch_add(n, std::memory_order_relaxed);
    m1Rate->update(n);
    m5Rate->update(n);
    m15Rate->update(n);
  }

  void tickIfNecessary() {
    uint64_t oldTick = lastTick;
    uint64_t newTick = clock.now().time_since_epoch().count();
    uint64_t age = newTick - oldTick;
    if (age > TICK_INTERVAL) {
      uint64_t newIntervalStartTick = newTick - age % TICK_INTERVAL;
      if (lastTick.compare_exchange_strong(oldTick, newIntervalStartTick)) {
        uint64_t requiredTicks = age / TICK_INTERVAL;
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
      double elapsed = (clock.now().time_since_epoch().count() - startTime);
      return getCount() / elapsed;
    }
  }
};
}