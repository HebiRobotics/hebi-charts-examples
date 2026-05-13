cmake_minimum_required(VERSION 3.14)
project(HebiChartsSample CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Single header library with a stable C ABI
include(hebi_charts/hebi_charts.cmake)

add_executable(ex01_randomwalk ex01_randomwalk.cpp)
target_add_hebi_charts(ex01_randomwalk)

add_executable(ex02_sinewave ex02_sinewave.cpp)
target_add_hebi_charts(ex02_sinewave)

add_executable(ex03_robot_3d ex03_robot_3d.cpp)
target_add_hebi_charts(ex03_robot_3d)

add_executable(ex04_latency ex04_latency.cpp)
target_add_hebi_charts(ex04_latency)

