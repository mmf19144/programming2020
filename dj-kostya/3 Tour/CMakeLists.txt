cmake_minimum_required(VERSION 3.17)
project(HashMap)

set(CMAKE_CXX_STANDARD 17)
#add_library(list )

add_executable(HashMap  main.cpp list.cpp list.h hash_map.cpp hash_map.h)
#target_link_libraries(HashMap list)