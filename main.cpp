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

#include <atomic>
#include <bits/types/struct_itimerspec.h>
#include <chrono>
#include <condition_variable>
#include <ctime>
#include <iostream>
#include <mutex>
#include <ostream>
#include <ratio>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <thread>
#include <vector>

#include "workers.hpp"

/* Basic tests
 */
void tests() {
  /* Default constructor uses one thread, acting as an event loop.
   */
  jlworkers::Workers eventLoop, workerThreads(4);

  /* Start both workers. They'll stand by and wait for tasks.
   */
  eventLoop.start();
  workerThreads.start();

  /* Tasks A and B can run in any order
   */
  workerThreads.post([] {
    std::cout << "Task A\n";
  });
  workerThreads.post([] {
    std::cout << "Task B\n";
  });

  /* Task C shall always run before task D
   */
  eventLoop.post([] {
    std::cout << "Task C\n";
  });
  eventLoop.post([] {
    std::cout << "Task D\n";
  });

  /* Tasks E and F will start after a timeout given in milliseconds
   */
  eventLoop.post_timeout([] {
    std::cout << "Task E\n";
  }, 1500);
  eventLoop.post_timeout([] {
    std::cout << "Task F\n";
  }, 1000);

  /* Wait for all threads to finish
   */
  eventLoop.join();
  workerThreads.join();
}

int main(int argc, const char** argv) {
  tests();  

  return 0;
}
