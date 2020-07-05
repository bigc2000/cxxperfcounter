#pragma  once

#include <memory.h>
#include <mutex>
#include "../typedefines.h"
#include "Clock.h"
#include "Metric.h"
#include "UniformSample.h"
/**
 * @brief simplify the implementation from JAVA. 
 * only has {size} buckets,and statistics last 10 minutes 
 *
 */
namespace mc {

static const int DEFAULT_SAMPLE_SIZE = 1028;
static const int DEFAULT_STEP_PER_TICK = 10;

// so after DEFAULT_SAMPLE_SIZE/DEFAULT_STEP_PER_TICK * TICK_INTERVAL seconds,the snapshot will decay to zero 

class Histogram {
  UniformSample sample;
  std::string _name;
  AtomicLong lastTick;
  
  public:
  Histogram() noexcept : sample(DEFAULT_SAMPLE_SIZE, DEFAULT_STEP_PER_TICK,(std::chrono::duration_cast<TIME_DURATION>(std::chrono::minutes(19))).count()), _name(){
    lastTick = GetNowTimeCount(); 

  }

  Histogram(const Histogram &src) noexcept :sample(src.sample), _name(src._name), lastTick(src.lastTick.load()) {
  }
  Histogram(const Histogram && src) noexcept :sample(std::move(src.sample)), _name(std::move(src._name)), lastTick(src.lastTick.load()) {
  }


  Histogram & operator =(const Histogram &src) noexcept {
    sample = src.sample;
    _name = (src._name);
    lastTick = (src.lastTick.load());
  }


  Histogram & operator =(const Histogram &&src) noexcept {
    sample = src.sample;
    _name = (src._name);
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

  void mark(double value) {
    INT64_T lastTime = GetNowTimeCount();
    sample.mark(lastTime, value);
  }

  void mark(int cnt, double value) {
    INT64_T  lastTime = GetNowTimeCount();
    for (int i = 0; i < cnt; i++) {
      sample.mark(lastTime, value);
    }
  }

  /**
  *@brief only used for test, the size may vary widely.
  *@return return the values of SAMPLE during[first ,last)
  */
  INT64_T getSize() const  {
    return (INT64_T)sample.size();
  };

  ///@brief don't call frequently.just call once,and get percentile.
  Snapshot getSnapshot() {
    tickIfNecessary();
    return std::move(sample.getSnapshot());
  }
  void tickIfNecessary() {
    //45sample.mark(0);
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
