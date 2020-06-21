#include <sstream>
#include "MetricRegistry.h"

#include "JsonSerialize.h"
#include "MetricAgent.h"
#include "../curl/CurlHttp.h"
using namespace mc;

HttpSendTask::HttpSendTask() : initialDelay(0), period(300), url(""), endpoint("onebox") {
}

HttpSendTask::~HttpSendTask() {
}

void HttpSendTask::run() {
  const std::string &req = this->write();
  std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(),
                                                           &curl_easy_cleanup);
  setDefaultCurlOpt(curl.get());
  std::string res;
  const char *url = "http://127.0.0.1:1988/v1/push";
  curl_easy_http_post(curl.get(), 3000, url, req, &res);
}

void HttpSendTask::writeCounter(std::stringstream &ss, CounterPtr ptr) const {
  METRIC_TYPE metricType = (ptr)->getType();
  time_t ts = time(NULL);
  switch (metricType) {
    case COUNTER:
      ss << "{\"endpoint\":\"" << endpoint << "\","
         << "\"metric\":\"" << "cps-1-min/" << ptr->name().c_str() << "\","
         << "\"step\":" << AGENT_UPLOAD_STEP_SEC << ","
         << "\"value\":" << ptr->getOneMinuteRate() << ","
         << "\"timestamp\":" << ts << ",";
      if (!ptr->tags().empty()) {
        ss << "\"tags\":\"" << Metric::formatTags(ptr->tags()) << "\",";
      }
      ss << "\"counterType\":\"GAUGE\""
         << "}";
      break;
    default:
      //error
      break;
  }
}

void percentile(std::stringstream &ss, const std::string &endpoint, const std::string &name,
                const Snapshot &snapshot,
                const std::map<std::string, std::string> tagMap,
                time_t ts, int percent) {
  double over = 100.0D;
  if (percent >= 100) {
    over = 1000.0D;
  }
  ss << "{\"endpoint\":\"" << endpoint << "\","
     << "\"metric\":\"" << percent << "-percentile/" << name.c_str() << "\","
     << "\"value\":" << snapshot.getValue(percent / over) << ","
     << "\"step\":" << AGENT_UPLOAD_STEP_SEC << ","
     << "\"timestamp\":" << ts << ",";
  if (!tagMap.empty()) {
    ss << "\"tags\":\"" << Metric::formatTags(tagMap) << "\",";
  }
  ss << "\"counterType\":\"GAUGE\""
     << "}";
}


void HttpSendTask::writeHistogram(std::stringstream &ss, HistogramPtr ptr) const {
  const Snapshot &snapshot = ptr->getSnapshot();
  METRIC_TYPE metricType = (ptr)->getType();
  time_t ts = time(NULL);
  switch (metricType) {
    case HIST:
      percentile(ss, endpoint, ptr->name(), snapshot, ptr->tags(), ts, 75);
      ss << ",";
      percentile(ss, endpoint, ptr->name(), snapshot, ptr->tags(), ts, 90);
      ss << ",";
      percentile(ss, endpoint, ptr->name(), snapshot, ptr->tags(), ts, 99);
      break;
    default:
      //error
      break;
  }
}

/**
 * "[{"endpoint":"oneboxhost","metric":"common/memoryq/micloud_pdc_data_0/size_total","step":300,"value":0,"counterType":"GAUGE","tags":"cluster=staging,cop=xiaomi,owt=miliao,service=micloudkitservice,pdl=micloud,job=micloudkitservice","timestamp":1569836926},{"endpoint":"oneboxhost","metric":"thriftsvr/call/count_pending","step":300,"value":0,"counterType":"GAUGE","tags":"cluster=staging,cop=xiaomi,owt=miliao,service=micloudkitservice,pdl=micloud,job=micloudkitservice","timestamp":1569836926}]
 * @brief
 * @return
 */
std::string HttpSendTask::write() {
  const MetricRegistry *registry = MetricRegistry::getInstance();
  std::stringstream ss;
  ss << "[";
  for (auto &ptr :registry->getMeters()) {
    if (ptr->getType() == COUNTER) {
      if (ss.tellg() > 10) {
        ss << ",";
      }
      writeCounter(ss, dynamic_cast<CounterPtr >(ptr));
    } else if (ptr->getType() == HIST) {
      if (ss.tellg() > 10) {
        ss << ",";
      }
      writeHistogram(ss, dynamic_cast<HistogramPtr >(ptr));
    }
  }
  ss << "]";
  return ss.str();
}
