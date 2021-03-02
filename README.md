# Event loop and worker thread

Usage:

    mkdir build
    cd build
    cmake ..
    cmake --build .
    ./eventloop

The Workers class has two constructors. The default constructor (no arguments) returns an instance utilising a single thread,
or rather, it runs one task at a time in the order the tasks were added to the queue. This makes the instance act as an event loop.

The other constructor takes one argument, giving the number of concurrent threads to use. Order of execution is not guaranteed.

The `post_timeout([](), int timeout)` method is nonblocking, and places the task in a separate queue before adding it to the main
queue after the timeout.
