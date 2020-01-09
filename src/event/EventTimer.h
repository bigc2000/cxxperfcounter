//
// Created by wx on 18-9-8.
//


#ifndef EVENT_TIMER_H
#define EVENT_TIMER_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <mutex>
#include <memory>
#include <vector>

#if defined(WIN32)
#define    WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <sys/timeb.h>
#define THREAD_RETURN void
#define usleep(x) sleep((x)*1000L)
#else

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>


#ifndef THREAD_ID
#define THREAD_ID pthread_t
#endif

#define THREAD_RETURN void*
#define GetCurrentThreadId pthread_self
#endif


#include "event2/event-config.h"
#include "event2/event_struct.h"
#include "event2/event.h"



/**
*@author wx
*based on libevent timer
*timer的实现是单个线程执行多个timertask，如果需要多线程执行，需要修改timer的设计
*/

namespace mc {
class Runnable {
public:
  Runnable() {}

  virtual ~Runnable() {}

  virtual void run()=0;
};

class CEvTimerTask : public Runnable {
public:
  enum {
    TASK_CREATED,
    TASK_WAITING,
    TASK_RUNING,
    TASK_STOPPED,
    TASK_REMOVED,
  };

  CEvTimerTask() : command(NULL), ev(NULL), status(0), times(1), fixedRate(false) {
    tv.tv_sec = 0;
    tv.tv_usec = 0;
  }

  virtual ~CEvTimerTask() {
    if (ev != NULL) {
      event_free(ev);
      ev = NULL;
    }
  }

  virtual void run() {
    if (command != NULL) {
      command->run();
    }
  }

  bool isClosed() {
    return times == 0;
  }

  Runnable *command;
  event *ev;
  struct timeval tv;
  volatile int status;
  int times;//执行次数，<0:不限制继续执行循环，0:这次都不执行就退出 1:仅这1次执行完后退出
  //是否是固定速率，默认为false.
  //分2种定时任务，一种是固定速率（即每隔多少秒即执行，如果任务时间很长，超过了间隔时间，那么下次执行为立即执行），
  //一种是固定间隔（即执行完成后，间隔多长时间执行）
  bool fixedRate;
};

class CEvSingleThreadTimer {

public:
  enum {
    TIMER_CREATE,
    TIMER_START,
    TIMER_RUNNING,
    TIMER_CANCEL,
    TIMER_STOPPED,
  };

  CEvSingleThreadTimer() : status(TIMER_CREATE), fd(0), eb(NULL) {

  }


  ~CEvSingleThreadTimer();

private:
  void start();

public:
  void cancel() {
    status = TIMER_CANCEL;
  }

  int getStatus() const {
    return status;
  }

  void setStatus(int status) {
    this->status = status;
  }

  event_base *getEventBase();

public:
  /**
   * @brief
   * @param command
   * @param initialDelay
   * @param period
   */
  void scheduleAtFixedRate(Runnable *command,
                           std::int64_t initialDelay,
                           std::int64_t period);

  void scheduleWithFixedDelay(Runnable *command,
                              std::int64_t initialDelay,
                              std::int64_t period);

  void shutdown() {
    cancel();
  }

  void shutdownNow() {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    int result = 0;
    int ret = 0;
    int detachState;
    ret = pthread_cancel(fd);
    if (ret < 0) {
      perror("cannot cancel thread");
    }
    status = TIMER_STOPPED;
  }

private:

  void addTimerTask(CEvTimerTask *pTask);

  void removeTask(CEvTimerTask *pTask);

private:
  CEvSingleThreadTimer(const CEvSingleThreadTimer &) = delete;

  CEvSingleThreadTimer operator=(const CEvSingleThreadTimer &) = delete;

private:

  volatile int status;//{0:创建，1已启动，2取消}isCancel;
  THREAD_ID fd;
  struct event_base *eb;
  std::vector<CEvTimerTask *> taskQueue;
  std::recursive_mutex m_mutex;
};
}
#endif //CXXPERFCOUNTER_EVENTTIMER_H
