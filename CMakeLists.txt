cmake_minimum_required(VERSION 3.10)
PROJECT(TextSumm)

set(CMAKE_CXX_FLAGS "-Wall")
add_executable(Main Main.cpp Summarizer.cpp)