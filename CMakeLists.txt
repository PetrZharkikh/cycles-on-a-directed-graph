cmake_minimum_required(VERSION 3.10)

project(LoopTree)

set(CMAKE_CXX_STANDARD 17)

add_executable(loop_tree
    src/main.cpp
    src/LoopFinder.cpp
)

target_include_directories(loop_tree PRIVATE include)