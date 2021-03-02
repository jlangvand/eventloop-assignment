#ifndef __WORKERS_HPP__
#define __WORKERS_HPP__

#include <atomic>
#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

namespace jlworkers {
  /* Worker thread/event loop class

     The default constructor will create an instance utilising a single thread
     for each task, making it act as an event loop. You can, however, specify
     any number of threads to run tasks concurrently.
   */
  class Workers {

  private:
    int m_maxThreadCount;
    std::atomic_int m_runningThreadCount;

    std::atomic_bool m_running;
    std::condition_variable m_runningCondition;
    std::mutex m_runningMutex;

    std::list<std::function<void()>> m_queue;
    std::vector<std::thread> m_workers;

    std::thread m_runnerThread;

  public:
    Workers();
    Workers(int numThreads);

    void post(const std::function<void ()>&);
    void post_timeout(const std::function<void ()>&, int timeout);
    void start();
    void join();
  };
}

#endif//__WORKERS_HPP__
