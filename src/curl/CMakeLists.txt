add_library(curlutil STATIC CurlHttp.cpp)
target_link_libraries(curlutil curl pthread)
