add_library(eventhttpserver STATIC LibeventHttpServer.cpp EventTimer.cpp)
target_link_libraries(eventhttpserver event pthread)
