//
// Created by wx on 18-9-10.
//

#pragma once

#include <chrono>
#ifndef CLOCK
#define CLOCK std::chrono::steady_clock 
#endif
//typedef decltype(CLOCK::now().time_since_epoch()) TIME_DURATION;
typedef std::chrono::seconds TIME_DURATION;


