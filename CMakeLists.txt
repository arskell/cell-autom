cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -pthread")

add_subdirectory(src)
add_subdirectory(tests)
