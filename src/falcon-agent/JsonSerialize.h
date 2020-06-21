#pragma once

#include <string>

#include "MetricRegistry.h"
#include "Metered.h"


#include <string>
#include <sstream>

namespace mc {
class HttpSendTask  {
  int initialDelay;//mill seconds
  int period; //mill seconds
  std::string url;
  std::string endpoint;

public:
  HttpSendTask();

  void setEndpoint(const std::string &endpoint) {
    this->endpoint = endpoint;
  }

  virtual ~HttpSendTask();

public:
  virtual void run();

private:
  std::string write();

  void writeCounter(std::stringstream &ss, CounterPtr ptr) const;

  void writeHistogram(std::stringstream &ss, HistogramPtr pCounter) const;
};
}