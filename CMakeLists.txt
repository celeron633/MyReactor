cmake_minimum_required(VERSION 3.0)
project(MyReactor)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS
    " -std=c++11 -g -O0 -Wall -Wextra -Wno-unused-parameter -Werror")

find_package(Threads REQUIRED)

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/base)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin)

add_subdirectory(base)
add_subdirectory(net)
