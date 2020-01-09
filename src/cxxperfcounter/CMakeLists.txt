add_library(cxxmetric STATIC EWMA.cpp Snapshot.cpp Meter.cpp PerfCounter.cpp Counter.cpp Histogram.cpp)
target_link_libraries(cxxmetric event pthread)
