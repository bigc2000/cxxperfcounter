#pragma  once

#include <memory.h>
#include <mutex>
#include "../typedefines.h"
#include "Clock.h"
#include "Metric.h"
#include "UniformSample.h"
/**
 * @brief 简化了JAVA的逻辑,参照旧的java实现,性能应该比最新版本基于LongAdder和ConcurrentSkipListMap的java差2-3倍
 *只有update方法是同步方法,其它不能保证同步;如需要,显式使用锁
 * 去掉全局的统计，对于metrics来说，全局的意义不大，使用一段时间回归值
 *
 */
namespace mc {

static const int DEFAULT_SAMPLE_SIZE = 1028;
static const int DEFAULT_STEP_PER_TICK = 10;

// so after DEFAULT_SAMPLE_SIZE/DEFAULT_STEP_PER_TICK * TICK_INTERVAL seconds,the snapshot will decay to zero 

class Histogram : public Metric {
  UniformSample sample;
  std::string _name;
  CLOCK clock;//时钟 
  AtomicLong lastTick;
public:
  Histogram() noexcept : sample(DEFAULT_SAMPLE_SIZE, DEFAULT_STEP_PER_TICK), _name(), clock(CLOCK()) {
    lastTick = GetNowTimeCount();
  }

  Histogram(const Histogram &src) noexcept :sample(src.sample), _name(src._name), clock(src.clock), lastTick(src.lastTick.load()) {

  }
  Histogram & operator =(const Histogram &src)noexcept {
    sample = src.sample;
    _name = (src._name);
    clock = (src.clock);
    lastTick = (src.lastTick.load());
  }
  virtual ~Histogram() {}

  const std::string &name() const {
    return _name;
  }

  void name(const std::string &name) {
    _name = name;
  }

  void name(std::string &&name) {
    _name = name;
  }

  /**
  * @brief Adds a recorded value.
  * @param value the length of the value
  */

  void update(double value) {
    INT64_T lastTime = GetNowTimeCount();
    sample.update(lastTime, value);
  }

  void update(int cnt, double value) {
    INT64_T  lastTime = GetNowTimeCount();
    for (int i = 0; i < cnt; i++) {
      sample.update(lastTime, value);
    }
  }

  virtual INT64_T getCount() const override {
    return (INT64_T)sample.size();
  };

  virtual METRIC_TYPE getType() const override {
    return HIST;
  }

  ///@brief don't call frequently.just call once,and get percentile.
  Snapshot getSnapshot() {
    tickIfNecessary();
    return std::move(sample.getSnapshot());
  }
  void tickIfNecessary() {
    //45sample.update(0);
    INT64_T oldTick = lastTick;
    INT64_T newTick = GetNowTimeCount();
    INT64_T age = newTick - oldTick;
    if (age > mc::TICK_INTERVAL) {
      INT64_T newIntervalStartTick = newTick - age % mc::TICK_INTERVAL;
      if (lastTick.compare_exchange_strong(oldTick, newIntervalStartTick)) {
        INT64_T requiredTicks = age / mc::TICK_INTERVAL;
        printf("should ticks = %lld\n", requiredTicks);
        sample.tick(requiredTicks, newTick, TIME_UNIT_ONE_SECOND);
      }
    }
  }
};
}
