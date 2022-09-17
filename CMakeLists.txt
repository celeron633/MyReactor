cmake_minimum_required(VERSION 3.0)
project(MyReactor)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    message("building with Linux!")
else()
    message( FATAL_ERROR "Only support Linux!" )
endif()

# CXX 11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# debug build
set(CMAKE_BUILD_TYPE debug)

# CXX flags
set(CMAKE_CXX_FLAGS
    " -std=c++11 -g -O0 -Wall -Wextra -Wno-unused-parameter -Werror")

# multi thread support
find_package(Threads REQUIRED)

# 'gcc -I' include directories
include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/base ${PROJECT_SOURCE_DIR}/net)

# binary output path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin)

# sub directories
add_subdirectory(base)
add_subdirectory(net)
add_subdirectory(test)
