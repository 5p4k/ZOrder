cmake_minimum_required(VERSION 3.6)
project(ZOrder)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES benchmark.cpp zorder.h)
add_executable(benchmark ${SOURCE_FILES})

set(DEMO_SOURCE_FILES zorder.h demo.cpp)
add_executable(demo ${DEMO_SOURCE_FILES})