#include "EventTimer.h"

using namespace mc;


static void evCallBack(evutil_socket_t fd, short flag, void *arg);

static THREAD_RETURN run(void *arg);

namespace mc {

}

CEvSingleThreadTimer::~CEvSingleThreadTimer() {
  if (eb != NULL) {
    event_base_free(eb);
    eb = NULL;
  }
  for (size_t i = 0; i < taskQueue.size(); i++) {
    delete taskQueue[i];
    taskQueue[i] = NULL;//may not execute.
  }
}

void CEvSingleThreadTimer::start() {
  if (status != TIMER_CREATE) {
    return;
  }
  eb = event_base_new();
  status = TIMER_START;
#if defined(WIN32)
  this->fd = _beginthread(run, 1024 * 64, this);
          if(fd==-1){
              perror("create thread error.");
              return;
          }
#else
  int ret = 0;
  pthread_attr_t attr;
  ret = pthread_attr_init(&attr);
  if (ret != 0) {
    fprintf(stderr, "Could not initialize attr of worker.\n");
  }

  ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  if (ret != 0) {
    fprintf(stderr, "Could not set detach pthread \n");
  }

  ret = pthread_attr_setstacksize(&attr, 256 * 1024);
  if (ret != 0) {
    fprintf(stderr, "Could not set stack size of worker.\n");
  }
  ret = pthread_create(&this->fd, &attr, run, this);
  if (ret != 0) {
    fprintf(stderr, "Could not create worker.\n");
  }
  ret = pthread_attr_destroy(&attr);
  if (ret != 0) {
    perror("Could not destroy attr of .\n");
    exit(-1);
  }
#endif
  status = TIMER_RUNNING;
}

void CEvSingleThreadTimer::scheduleAtFixedRate(Runnable *command,
                                               std::int64_t initialDelay,
                                               std::int64_t period) {

  CEvTimerTask *pTask = new CEvTimerTask;
  pTask->times = -1;
  pTask->fixedRate = true;
  pTask->command = command;
  pTask->tv.tv_sec = period / 1000;
  pTask->tv.tv_usec = (period % 1000) * 1000;

  std::lock_guard<decltype(m_mutex)> lock(m_mutex);
  if (status == TIMER_CREATE) {
    start();
  }
  addTimerTask(pTask);
}

void CEvSingleThreadTimer::scheduleWithFixedDelay(Runnable *command,
                                                  std::int64_t initialDelay,
                                                  std::int64_t period) {
  CEvTimerTask *pTask = new CEvTimerTask;
  pTask->times = -1;
  pTask->fixedRate = false;
  pTask->command = command;
  pTask->tv.tv_sec = period / 1000;
  pTask->tv.tv_usec = (period % 1000) * 1000;
  std::lock_guard<decltype(m_mutex)> lock(m_mutex);
  if (status == TIMER_CREATE) {
    start();
  }
  addTimerTask(pTask);
}

/**
*@brief 回调函数,如果是最后一次调用，那么将删除该task
*/
void evCallBack(evutil_socket_t fd, short flag, void *arg) {
  CEvTimerTask *pTask = (CEvTimerTask *) arg;
  if (pTask != NULL) {
    time_t t_before = 0;
    time_t t_after = 0;
    time(&t_before);
    pTask->run();
    time(&t_after);
    if (pTask->times > 0) {
      pTask->times--;
    }
    if (pTask->times == 0) {
      return;
    } else {
      int iRet = 0;
      iRet = evtimer_del(pTask->ev);
      if (iRet < 0) {
        fprintf(stderr, "error del timer task");
      }
      timeval tv = pTask->tv;
      if (pTask->fixedRate) {
        int elapsedTime = (t_after - t_before);
        tv.tv_sec -= elapsedTime;
        if ((int) tv.tv_sec < 0) {
          tv.tv_sec = 0;
        }
      }
      iRet = evtimer_add(pTask->ev, &(tv));
      if (iRet < 0) {
        fprintf(stderr, "error add timer task");
      }
    }
  }
}


/**
*@brief 线程执行的函数入口，如果直接启动，那么即在主线程中启动了监控，可能会导致主线程阻塞
*@param arg thread parameter,this pointer
*/

static THREAD_RETURN run(void *arg) {
  //ConsoleTrace::TraceWithTm("start timer=%d,thread=\n",GetCurrentThreadId());
  CEvSingleThreadTimer *timer = (CEvSingleThreadTimer *) arg;
  int iRet = 0;
  if (timer != NULL) {
    while (1) {
      if (timer->getStatus() == CEvSingleThreadTimer::TIMER_CANCEL) {
        event_base_loopbreak(timer->getEventBase());
        break;
      } else {
        iRet = event_base_loop(timer->getEventBase(), EVLOOP_ONCE | EVLOOP_NONBLOCK);
        if (iRet == 1) {
          usleep(100 * 1000);
        } else {
          usleep(10 * 1000);
        }
      }

    }
    timer->setStatus(CEvSingleThreadTimer::TIMER_STOPPED);
  }
#if defined WIN32
#else
  return NULL;
#endif
}

void CEvSingleThreadTimer::addTimerTask(CEvTimerTask *pTask) {
  if (status == TIMER_CREATE) {
    start();
  }
  struct event *ev = evtimer_new(eb, evCallBack, pTask);
  pTask->ev = ev;
  evtimer_add(ev, &pTask->tv);
  taskQueue.push_back(pTask);
}

void CEvSingleThreadTimer::removeTask(CEvTimerTask *pTask) {
  if (pTask != NULL) {
    int iRet = evtimer_del(pTask->ev);
    if (0 == iRet) {
      pTask->status = CEvTimerTask::TASK_REMOVED;
      return;
    }
  }
  fprintf(stderr, "remove timer task failed");
}

event_base *CEvSingleThreadTimer::getEventBase() {
  return eb;
}
