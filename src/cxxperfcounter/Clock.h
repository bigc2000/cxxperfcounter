//
// Created by wx on 18-9-10.
//

#pragma once

#include <chrono>

#ifndef MC_CLOCK
#define MC_LOCK
typedef std::chrono::steady_clock CLOCK;
typedef decltype(CLOCK::now()) TIME_POINT;
typedef decltype(CLOCK::now().time_since_epoch()) TIME_DURATION;

#endif

