//
// Created by wx on 18-8-11.
//
#pragma once

#include "../typedefines.h"
#include <string>
#include "Counter.h"

namespace mc {
class PerfCounter {

public:
  static INT64_T count(const std::string &name, int cnt);
  static void count(const std::string &name, int cnt, double timecost);
  static void countDuration(const std::string &name, double timecost);//TimeUnit.millseconds
};
}