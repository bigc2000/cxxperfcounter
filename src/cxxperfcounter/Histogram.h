#pragma  once

#include <memory.h>
#include <mutex>

#include <UniformSample.h>
/**
 * @brief 简化了JAVA的逻辑,参照旧的java实现,性能应该比最新版本基于LongAdder和ConcurrentSkipListMap的java差2-3倍
 *只有update方法是同步方法,其它不能保证同步;如需要,显式使用锁
 * 去掉全局的统计，对于metrics来说，全局的意义不大，使用一段时间回归值
 *
 */
namespace mc {
class Hisgogram;

static const int DEFAULT_SAMPLE_SIZE = 1028;

class Histogram : public Metric {
  UniformSample sample;
  std::string _name;
  AtomicULong lastTick;
  CLOCK clock;//时钟  
public:
  Histogram() : sample(DEFAULT_SAMPLE_SIZE) {
  }

  Histogram(const Histogram &src) = delete;

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
  void update(int value) {
      update((long) value);
  }

  /**
   * @brief 直接用粗粒度锁同步,而不是spin lock
   */
  void update(int64_t value) {
    sample.update(value);
  }

  virtual int64_t getCount() const override {
    return sample.size();
  };

  virtual METRIC_TYPE getType() const override {
    return HIST;
  }

  ///@brief don't call frequently.just call once,and get percentile.
  Snapshot getSnapshot() {
    return sample.getSnapshot();
  }
  void tickIfNecessary() {
    
  }
};
}
