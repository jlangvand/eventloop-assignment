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
    std::atomic_int m_runningTimeoutThreadCount;

    std::atomic_bool m_running;
    std::condition_variable m_runningCondition;
    std::mutex m_runningMutex;

    std::list<std::function<void()>> m_queue;
    std::vector<std::thread> m_workers;

    std::thread m_runnerThread;

    void stop();

  public:
    Workers();
    Workers(int numThreads);
    ~Workers() { stop(); }

    void post(const std::function<void ()>&);
    void post_timeout(const std::function<void ()>&, int timeout);
    void start();
    void join();
  };
}

#endif//__WORKERS_HPP__
