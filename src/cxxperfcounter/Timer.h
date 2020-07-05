//
// Created by wx on 19-6-10.
//
#pragma  once
#include "Meter.h"
#include "EWMA.h"
#include "Histogram.h"
namespace mc {
class Timer : public Meter {

  
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
   * @brief
   * @param cost  mark an event with cost 
   * @return origin value before add.
   */
  void mark(double cost) {
    Meter::mark(1);
    histogram.mark(cost);
  }
  
  /**
   * @brief fetch and add
   * @param n to add
   * @return origin value before add.
   */
  void mark(int count, double cost) {
    for (int i = 0; i < count; i++) {
      mark(cost);
    }
  }

  METRIC_TYPE getType() const override {
    return TIMER;
  }

  const Snapshot& getSnapshot() {
    return histogram.getSnapshot();
  }
  
};
}