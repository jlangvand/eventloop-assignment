#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <ostream>
#include <ratio>
#include <thread>

#include "workers.hpp"

void helloWorld() {
  std::cout << "Task defined as regular (static) function\n";
}

void workersTest(int threads) {
  jlworkers::Workers workerThread(threads);
  std::mutex printMutex;

  /*workerThread.post([]() {
    std::cout << "Hello, World!\n";
    });*/

  workerThread.post(helloWorld);
  
  workerThread.start();

  workerThread.post([]() {
    // Let's delay this a bit
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "Anonymous function with 500ms sleep before print\n";
  });

  for (int i = 0; i < 10; i++)
    workerThread.post([i] {
      std::cout << "Task " << i
                << ", thread id " << std::this_thread::get_id() <<"\n";
    });

  workerThread.stop();
}

void stuff() {
    std::atomic_bool wait(true);
  std::mutex mtx;
  std::condition_variable cv;

  std::thread t([&wait, &mtx, &cv] {
    //while (wait);
    std::unique_lock<std::mutex> lock(mtx);
    while (wait)
      cv.wait(lock);
    
    std::cout << "Thread finished" << std::endl;
  });
  //  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::this_thread::sleep_for(std::chrono::seconds(1));
  {
    std::unique_lock<std::mutex> lock(mtx);
    wait = false;
  }
  
  cv.notify_all();
  t.join();
}

int main(int argc, const char** argv) {

  // Test workerthread
  workersTest(16);

  // Test eventloop
  workersTest(1);
  
  
  return 0;
}
