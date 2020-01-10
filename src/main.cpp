
#include <iostream>
#include <cstring>
#include <curl/curl.h>
//#include "cxxperfcounter/EWMA.h"
#include <glog/logging.h>
#include <thread>
#include "falcon-agent/JsonSerialize.h"

#include "event/LibeventHttpServer.h"
#include "cxxperfcounter/PerfCounter.h"

void test(struct evhttp_request *req, void *) {
  printf("get request\n");
}

//mc_http_url_mapping_t test_url_mapping;


int main() {
  using namespace mc;
  mc::EWMA ewma = mc::EWMA::oneMinuteEWMA();
  ewma.tick();
  mc::EWMA ewma2 = mc::EWMA::oneMinuteEWMA();
  ewma2.tick();
  mc::CEvSingleThreadTimer timer;
  std::unique_ptr<HttpSendTask> pTask(new HttpSendTask);
  timer.scheduleAtFixedRate(pTask.get(), 10000, 10000);

  CURLcode curLcode = curl_global_init(CURL_GLOBAL_ALL);
  if (curLcode != CURLE_OK) {
    LOG(ERROR) << "curl init error:" << curLcode;
    exit(-1);
  }

  int c = 0;
  srand(0);

  //timer.scheduleAtFixedRate(MetricRegistry::getInstance(), 100, 300 * 1000);
  MetricRegistry::getInstance()->counter("abc")->inc(100);
  MetricRegistry::getInstance()->histogram("abc")->update(100);
  std::at_quick_exit(&curl_global_cleanup);
  int i = 0;
  while (1) {

    if (c++ % 5 == 0) {
      ewma.tick();
      printf("%f\n", ewma.getRate());
      for (const auto &it : MetricRegistry::getInstance()->getMeters()) {
        printf("i=%d,%ld\n", i, (it)->getCount());
      }
      printf("test:five minute rate=%f\n", MetricRegistry::getInstance()->counter("test")->getFiveMinuteRate());
      printf("abc:five minute rate=%f\n", MetricRegistry::getInstance()->counter("abc")->getFiveMinuteRate());
      PerfCounter::countDuration("ag", 1, rand() % 10);
      PerfCounter::countDuration("ah", rand() % 10, rand() % 1000);
      printf("ah:five minute snapshot=%s\n",
             MetricRegistry::getInstance()->histogram("ah")->getSnapshot().toString().c_str());
      printf("ag:five minute snapshot=%s\n",
             MetricRegistry::getInstance()->histogram("ag")->getSnapshot().toString().c_str());


      MetricRegistry::getInstance()->histogram("ah")->getSnapshot().dump(stdout);

    }
    if (i++ < 20) {
      PerfCounter::count("test", rand() % 10);
      PerfCounter::countDuration("ah", 1, rand() % 1000);
    }

    PerfCounter::countDuration("test/c/counter", 1, rand() % 1000);

    PerfCounter::count("test.c.counter", rand() % 10);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}