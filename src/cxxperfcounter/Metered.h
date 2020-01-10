#pragma once

#include <cstdint>

#include "Metric.h"

namespace mc {


class Metered : public Metric {
public:
  virtual ~Metered() {

  }

  /**
   * Returns the number of events which have been marked.
   *
   * @return the number of events which have been marked
   */
  virtual int64_t getCount() const = 0;

  /**
   * Returns the fifteen-minute exponentially-weighted moving average rate at which events have
   * occurred since the meter was created.
   * <p/>
   * This rate has the same exponential decay factor as the fifteen-minute load average in the
   * {@code top} Unix command.
   *
   * @return the fifteen-minute exponentially-weighted moving average rate at which events have
   *         occurred since the meter was created
   */
  virtual double getFifteenMinuteRate() = 0;

  /**
   * Returns the five-minute exponentially-weighted moving average rate at which events have
   * occurred since the meter was created.
   * <p/>
   * This rate has the same exponential decay factor as the five-minute load average in the {@code
   * top} Unix command.
   *
   * @return the five-minute exponentially-weighted moving average rate at which events have
   *         occurred since the meter was created
   */
  virtual double getFiveMinuteRate() = 0;

  /**
   * Returns the mean rate at which events have occurred since the meter was created.
   *
   * @return the mean rate at which events have occurred since the meter was created
   */
  virtual double getMeanRate() = 0;

  /**
   * Returns the one-minute exponentially-weighted moving average rate at which events have
   * occurred since the meter was created.
   * <p/>
   * This rate has the same exponential decay factor as the one-minute load average in the {@code
   * top} Unix command.
   *
   * @return the one-minute exponentially-weighted moving average rate at which events have
   *         occurred since the meter was created
   */
  virtual double getOneMinuteRate() = 0;
};

}
