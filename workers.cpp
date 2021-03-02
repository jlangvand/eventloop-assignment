/*
 * Copyright (C) 2021  Joakim Skogø Langvand
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>

#include "workers.hpp"

namespace jlworkers {
  /* Construct an event loop
   */
  Workers::Workers() {
    m_running = false;
    m_runningThreadCount = 0;
    m_runningTimeoutThreadCount = 0;
    m_maxThreadCount = 1;
  }

  /* Construct a concurent worker thread
   */
  Workers::Workers(int numThreads) {
    m_running = false;
    m_runningThreadCount = 0;
    m_runningTimeoutThreadCount = 0;
    // Ensure the number of threads is sane
    m_maxThreadCount = numThreads > 0 ? numThreads : 1;
  }

  /* Add a task to the queue
   */
  void Workers::post(const std::function<void()>& f) {
    std::unique_lock<std::mutex> lock(m_runningMutex);
    m_queue.emplace_back(f);
    m_runningCondition.notify_all();
  }

  /* Crude implementation of timeout method.
   */
  void Workers::post_timeout(const std::function<void()>& f, int timeout) {
    m_runningTimeoutThreadCount++;
    m_workers.emplace_back(std::thread([this, f, timeout] {
      std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
      post(f);
      std::unique_lock<std::mutex> lock(m_runningMutex);
      m_runningTimeoutThreadCount--;
      m_runningCondition.notify_all();
    }));
  }

  /* Start the main thread
   */
  void Workers::start() {
    std::cout << "Starting worker thread, " << m_maxThreadCount
              <<  " thread(s)\n";

    m_running = true;
    m_runnerThread = std::thread([this] {
      while (m_running) {
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
        std::unique_lock<std::mutex> lock(m_runningMutex);
        m_runningCondition.wait(lock);
      }
      for (; !m_workers.empty(); m_workers.pop_back()) m_workers.back().join();
    });
  }

  /* Because the spec states there should be a stop() method
   */
  void Workers::stop() {
    if (m_running)
      join();
  }

  void Workers::join() {
    /* Sohuld we wait for queue to clear, or do we want to stop adding tasks?
     * Let's do the former for now..
     */
    while (!m_queue.empty() || m_runningTimeoutThreadCount) {
      std::unique_lock<std::mutex> lock(m_runningMutex);
      m_runningCondition.wait(lock);
    }

    // Scope this to avoid deadlock
    {
      std::unique_lock<std::mutex> lock(m_runningMutex);
      m_running = false;
      m_runningCondition.notify_all();
    }
    
    m_runnerThread.join();  
  }
}

