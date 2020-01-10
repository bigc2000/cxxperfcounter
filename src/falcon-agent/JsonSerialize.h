#pragma once

#include <string>

#include "MetricRegistry.h"
#include "Metered.h"
#include "../event/EventTimer.h"

#include "../curl/CurlHttp.h"
#include <string>
#include <sstream>

namespace mc {
class HttpSendTask : public mc::Runnable {
  int initialDelay;//mill seconds
  int period; //mill seconds
  std::string url;
  std::string endpoint;

public:
  HttpSendTask() noexcept;
  HttpSendTask(const HttpSendTask&) noexcept;

  virtual ~HttpSendTask();

public:
  virtual void run() override;

  void setUrl(const std::string &url) {
    this->url = url;
  }

  const std::string &getUrl() const {
    return url;
  }

  std::string getUrl() {
    return url;
  }

  void setEndpoint(const std::string &endpoint) {
    this->endpoint = endpoint;
  }

  const std::string &getEndpoint() const {
    return endpoint;
  }

  std::string getEndpoint() {
    return endpoint;
  }


private:
  std::string write();

  void writeCounter(std::stringstream &ss, CounterPtr ptr) const;

  void writeHistogram(std::stringstream &ss, HistogramPtr pCounter) const;
};
}