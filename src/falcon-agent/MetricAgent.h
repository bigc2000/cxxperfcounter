//
// Created by wx on 19-6-6.
//

#pragma  once

#include <string>
#include <vector>
#include "MetricRegistry.h"
#include "Metric.h"
#include "NetUtil.h"

namespace mc {
const static int AGENT_UPLOAD_STEP_SEC = 300;

class MetricRecord {
public:
  std::string endpoint;
  std::string metric;
  int step;
  std::string counterType;
  std::string tags;
  int64_t timestamp;

public:
  MetricRecord() : step(AGENT_UPLOAD_STEP_SEC), counterType("GAUGE") {};

  MetricRecord(const std::string endpoint_, const std::string &metric_, int step_, const std::string &counterType_,
               const std::string &tags_,
               int64_t timeshamp_)
      : endpoint(endpoint_), metric(metric_), step(step_), counterType(counterType_), tags(tags_),
        timestamp(timeshamp_) {};

};

class FalconAgent {

  static std::map<std::string, std::string> tags;
public:
  static void initDefaultTags(const std::string xboxTag) {
    //parse
    std::string group = "test";
    tags.insert(std::pair<std::string, std::string>("group", group));
  }

  /// @brief buf IN&OUT
  static int write(char *buf, int off, int maxLen, CounterPtr ptr) {
    int pos = 0;
    METRIC_TYPE metricType = (ptr)->getType();
    switch (metricType) {
      case COUNTER:
        pos += snprintf(buf + off + pos, maxLen - (off + pos), "{\"endpoint\":\"%s\",", ptr->name().c_str());
        pos += snprintf(buf + off + pos, maxLen - (off + pos), "\"step\":\"%d\",", AGENT_UPLOAD_STEP_SEC);
        pos += snprintf(buf + off + pos, maxLen - (off + pos), "\"value\":\"%f\",", ptr->getOneMinuteRate());
        pos += snprintf(buf + off + pos, maxLen - (off + pos), "\"counterType\":\"GAUGE\",");
        pos += snprintf(buf + off + pos, maxLen - (off + pos), "\"timestamp\":\"%ld\"}", 0L);


//
//        jsonStream << "{";
//        jsonStream << "\"endpoint\":\"onebox\",";
//        jsonStream << "\"step\":300,";
//        jsonStream << "\"name\":\"test-my-cxx-perf\",";
//        jsonStream << "\"value\":" << it->getCount() << ",";
//        jsonStream << "\"counterType\": \"GAUGE\",";
//        jsonStream << "\"timestamp\":" << 1559786399;
//        jsonStream << "}";
//        jsonStream << ",";
        break;
      case GAUGE:
        break;
    }
    return pos;
  }

  //[IN &OUT]
  static bool flatAppend(std::vector<MetricRecord> &outVec, mc::Metric *metric) {
    Counter *counterPtr = NULL;
    switch (metric->getType()) {
      case COUNTER:
        counterPtr = dynamic_cast<Counter *>(metric);
//        MetricRecord oneMinRate;
//        oneMinRate.metric = "cps-1-min";
//        oneMinRate.endpoint = NetUtil::getHost();
//        oneMinRate.counterType = "GAUGE";
//        oneMinRate.tags = "group=oneboxhost";
        outVec.emplace_back(MetricRecord(
            NetUtil::getHost(),
            "cps-1-min",
            AGENT_UPLOAD_STEP_SEC,
            "GAUGE",
            "group=oneboxhost",
            0
        ));
        outVec.emplace_back(MetricRecord(
            NetUtil::getHost(),
            "cps-15-min",
            AGENT_UPLOAD_STEP_SEC,
            "GAUGE",
            "group=oneboxhost",
            0
        ));
        break;
      case HIST:
        break;
      case GAUGE:
        break;
    }
    return true;
  }
};
}