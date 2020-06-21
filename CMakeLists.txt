cmake_minimum_required(VERSION 3.5)
project("perfcounter")

set(CMAKE_CXX_STANDARD 11)
#set(SOURCE_FILES src/cxxperfcounter/MetricRegistry.cpp src/cxxperfcounter/MetricRegistry.h src/cxxperfcounter/Counter.cpp src/cxxperfcounter/Counter.h src/cxxperfcounter/Metered.h src/cxxperfcounter/MetricProcessor.cpp src/cxxperfcounter/MetricProcessor.h src/cxxperfcounter/EWMA.h src/typedefines.h src/cxxperfcounter/PerfCounter.cpp src/cxxperfcounter/PerfCounter.h src/cxxperfcounter/Snapshot.cpp src/cxxperfcounter/Snapshot.h src/cxxperfcounter/Sampling.h src/cxxperfcounter/Histogram.cpp src/cxxperfcounter/Histogram.h src/cxxperfcounter/Meter.cpp src/cxxperfcounter/Meter.h)

include_directories(src/event src/cxxperfcounter src/falcon-agent src/curl third_party)

include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})
#set(SOURCE_FILES src/event/LibeventHttpServer.c)

#add_subdirectory(src/event)

add_subdirectory(src/cxxperfcounter)
if(WITH_CURL)
   add_subdirectory(src/curl)
endif()
if(WITH_FALCON_AGENT)
  add_subdirectory(src/falcon-agent)
endif()

#find_library(eventhttpserver src/event)
#find_library(eventhttpserver ${CMAKE_CURRENT_SOURCE_DIR}/event)

# 基于static方式来做的
#add_library(glog_static STATIC IMPORTED)
#set_target_properties(glog_static PROPERTIES IMPORTED_LOCATION /usr/local/lib/libglog.a)
#
#add_library(event_static STATIC IMPORTED)
#add_library(event_core_static STATIC IMPORTED)
#add_library(event_extra_static STATIC IMPORTED)
#set_target_properties(event_static event_core_static event_extra_static PROPERTIES IMPORTED_LOCATION /usr/local/lib/libevent.a)
#
#add_library(gflag_static STATIC IMPORTED)
#set_target_properties(gflag_static PROPERTIES IMPORTED_LOCATION /usr/local/lib/libgflags.a)
#
#


#target_link_libraries(perfcounter glog_static gflag_static)
#target_link_libraries(perfcounter event_static event_core_static event_extra_static)
# 动态链接


add_executable(cxxperfcounter_test src/main.cpp )
target_link_libraries(cxxperfcounter_test cxxmetric )