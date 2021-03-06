
#include <iostream>
#include <cstring>
#include "cxxperfcounter/EWMA.h"

#include <thread>
#include "cxxperfcounter/PerfCounter.h"
#include "cxxperfcounter/MetricRegistry.h"


void test(struct evhttp_request *req, void *) {
  printf("get request\n");
}

struct B {
  int n;
  
  B() {
    printf("default::B\n");
  }

  B(const B&) {
    printf("const ctor::B\n");
  }
  B( B&&) {
    printf("R  ctor::B\n");
  }

};


struct A {
  int n;
  B b;
  A() {
    printf("default :A\n");
  }
  A(const A&a):b(a.b) {
    printf("const ctor\n");
  }
  A(A&&a):b(std::move(a.b)) {
    printf("R  ctor A:\n");
  }


  const B& getB()const {
    printf("get const &() ::A");
    return b;
  }
};


int main() {
  A *pA = new A();
  A a2(*pA);
  A a3(std::move(*pA));

 
  const B&b2 = std::move(a2.getB());
  const B&b3(a2.getB());

 

  decltype(CLOCK::now().time_since_epoch()) nowTime = CLOCK::now().time_since_epoch();
  INT64_T newTick = nowTime.count();

  auto &UNIT_PER_SECONDS = std::chrono::duration_cast<decltype(CLOCK::now().time_since_epoch())>(std::chrono::seconds(1));
  printf("UNIT = %lld,newTick=%lld", UNIT_PER_SECONDS.count(), newTick);


  using namespace mc;
  std::unique_ptr<mc::EWMA> ewma(mc::EWMA::oneMinuteEWMA());
  ewma->tick();
  std::unique_ptr<mc::EWMA> ewma2(mc::EWMA::oneMinuteEWMA());
  ewma2->tick();
  int c = 0;
  srand(0);
  using namespace mc;
  //timer.scheduleAtFixedRate(MetricRegistry::getInstance(), 100, 300 * 1000);
  MetricRegistry::getInstance()->counter("abc")->inc(100);
  MetricRegistry::getInstance()->timer("abc")->mark(100);
  int i = 0;
  while (1) {

    if (c++ % 5 == 0) {
      ewma->tick();
      printf("%f\n", ewma->getRate());
      for (const auto &it : MetricRegistry::getInstance()->getMeters()) {
        printf("i=%d,%lld\n", i, (it)->getCount());
      }
      printf("test:five minute rate=%f\n", MetricRegistry::getInstance()->counter("test")->getFiveMinuteRate());
      printf("abc:five minute rate=%f\n", MetricRegistry::getInstance()->counter("abc")->getFiveMinuteRate());
      PerfCounter::count("ag", 1, rand() % 10);
      PerfCounter::countDuration("ah", rand() % 1000);
      printf("snapshot size=%d\n", MetricRegistry::getInstance()->timer("ah")->getFifteenMinuteRate());

      //printf("ah:five minute snapshot=%s\n",
      //       MetricRegistry::getInstance()->timer("ah")->getSnapshot().toString().c_str());
      //printf("ag:five minute snapshot=%s\n",
      //       MetricRegistry::getInstance()->timer("ag")->getSnapshot().toString().c_str());


    }
    if (i++ < 20) {
      PerfCounter::count("test", rand() % 10);
      PerfCounter::count("ah", 1, rand() % 1000);
    }

    PerfCounter::count("test/c/counter", 1, rand() % 1000);

    PerfCounter::count("test.c.counter", rand() % 10);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}