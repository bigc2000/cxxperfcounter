add_library(eventhttpserver STATIC EventTimer.cpp)
target_link_libraries(eventhttpserver event pthread)
