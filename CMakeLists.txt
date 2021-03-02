cmake_minimum_required(VERSION 3.10)

# set the project name
project(Eventloop VERSION 0.0.1)

# add the executable
add_executable(eventloop main.cpp workers.cpp)

set(CMAKE_CXX_FLAGS "-lpthread")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
