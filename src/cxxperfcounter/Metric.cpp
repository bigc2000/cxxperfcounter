#include "Metric.h"
decltype(TIME_DURATION(1).count()) mc::GetNowTimeCount(){
  return std::chrono::duration_cast<TIME_DURATION>(CLOCK::now().time_since_epoch()).count();
}
