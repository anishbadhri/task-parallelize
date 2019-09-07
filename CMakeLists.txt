cmake_minimum_required(VERSION 3.10)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

add_library(TaskParallelize library.cpp library.h)
add_executable(Runnable main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
