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

#include <iostream>

#include "workers.hpp"

/* Basic tests
 */
void tests() {
  std::cout << "\nExample event loop and worker thread implementation.\n\n"
            << "We'll run 6 tasks, A through F, where A and B runs in a\n"
            << "worker thread, C through F in an event loop.\n\n"
            << "Tasks E and F are started with an initial delay of 1500ms and\n"
            << "1000ms, respectively.\n\n"
            << "This means A and B can run at any time, in any order. C will\n"
            << "run before D, E will be added to the queue 500ms after F\n\n";

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

  // ~Workers does GC, we can safely exit the scope
}

int main(int argc, const char** argv) {
  tests();  

  return 0;
}
