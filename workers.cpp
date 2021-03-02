#include <condition_variable>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <utility>

#include "workers.hpp"

namespace jlworkers {
  Workers::Workers() {
    this->m_running = false;
    this->m_runningThreadCount = 0;
    this->m_maxThreadCount = 1;
  }

  Workers::Workers(int numThreads) {
    this->m_running = false;
    this->m_runningThreadCount = 0;
    // Ensure the number of threads is sane
    this->m_maxThreadCount = numThreads > 0 ? numThreads : 1;
  }
  
  void Workers::post(const std::function<void()>& f) {
    //std::cout << "Adding task to queue, size before: " << m_queue.size();
    std::unique_lock<std::mutex> lock(m_runningMutex);
    this->m_queue.emplace_back(f);
    m_runningCondition.notify_all();
    //std::cout << ", size after: " << m_queue.size() << "\n";
  }

  void Workers::start() {
    m_running = true;
    std::cout << "Starting task runner thread\n";
    m_runnerThread = std::thread([this] {
      while (m_running) {
        std::cout << "Queue: " << m_queue.size() << "\n";
        //while (!m_queue.empty()) {
          while (m_runningThreadCount < m_maxThreadCount && !m_queue.empty()) {
            // Fetch next task in queue
            auto f = *m_queue.begin();
            // Keep track of running threads
            m_runningThreadCount++;
            m_queue.pop_front();
            m_workers.emplace_back(std::thread([this, f] {
              // Run the task, blocking this thread
              f();
              std::unique_lock<std::mutex> lock(m_runningMutex);
              m_runningThreadCount--;
              m_runningCondition.notify_all();
            }));
          }
          //}

        // Wait for any status updates
        std::cout << "Create lock and wait for update\n";
        std::unique_lock<std::mutex> lock(m_runningMutex);
        m_runningCondition.wait(lock);
      }
      std::cout << "Queue empty\n";
      for (; !m_workers.empty(); m_workers.pop_back()) m_workers.back().join();
    });
  }

  void Workers::stop() {
    /* Sohuld we wait for queue to clear, or do we want to stop adding tasks?
     * Let's do the latter for now..
     */
    std::cout << "Stop runner thread; locking mutex\n";

    while (!m_queue.empty()) {
      std::unique_lock<std::mutex> lock(this->m_runningMutex);
      m_runningCondition.wait(lock);
    }

    // Scope this to avoid deadlock
    {
      std::unique_lock<std::mutex> lock(this->m_runningMutex);
      this->m_running = false;
      std::cout << "Notify threads\n";
      m_runningCondition.notify_all();
    }
    
    std::cout << "Wait for runner to join\n";
    m_runnerThread.join();
    std::cout << "Runner done!\n";
  }
}

