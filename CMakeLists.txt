cmake_minimum_required(VERSION 3.6)
project(permutation)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp permutation.h)
add_executable(permutation ${SOURCE_FILES})