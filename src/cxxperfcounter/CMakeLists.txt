add_library(cxxmetric STATIC EWMA.cpp Snapshot.cpp Metric.cpp Meter.cpp PerfCounter.cpp Counter.cpp Histogram.cpp Timer.cpp PerfCounter.cpp)
target_link_libraries(cxxmetric )