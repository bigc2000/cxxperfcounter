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
  static int64_t count(const std::string &name, int cnt);

  static void countDuration(const std::string &name, int cnt, int timecost);//TimeUnit.millseconds
};
}