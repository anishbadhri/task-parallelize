cmake_minimum_required(VERSION 3.10)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

add_library(TaskParallelize library.cpp library.h)

add_subdirectory(examples)