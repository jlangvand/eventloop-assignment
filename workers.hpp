#ifndef __WORKERS_HPP__
#define __WORKERS_HPP__

#include <atomic>
#include <condition_variable>
#include <functional>
#include <list>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

namespace jlworkers {
  class Workers {

  private:
    void m_workerFunc() {
      
    };
    std::map<std::thread, std::atomic_bool> m_workers_status;
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
