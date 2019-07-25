cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 14)


if(UNIX)
    add_definitions(-DISUNIX)
endif()

add_subdirectory(src)
add_subdirectory(tests)
