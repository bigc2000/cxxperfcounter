//
// Created by wx on 18-9-10.
//

#ifndef CXXPERFCOUNTER_GAUGE_H
#define CXXPERFCOUNTER_GAUGE_H

#include "../typedefines.h"
#include "Metered.h"
#include "Meter.h"
namespace mc {
class Gauge : public Meter {

private:
  AtomicLong gauge;
public:
  Gauge() {}

  virtual ~Gauge() {}

  int64_t getValue() const {
    return gauge;
  }

  METRIC_TYPE getType() const {
    return GAUGE;
  }
};
}

#endif //CXXPERFCOUNTER_GAUGE_H
