cmake_minimum_required(VERSION 3.0)
project(MyReactor)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    message("building with Linux!")
else()
    message(FATAL_ERROR "Only support Linux!")
endif()

# CXX 11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# debug build
set(CMAKE_BUILD_TYPE debug)

# CXX flags
set(CMAKE_CXX_FLAGS
    "-std=c++11 -g -O0 -Wall -Wextra -Wno-unused-parameter -Werror -Wno-unused-variable")

# use epoll as default
add_definitions(-DMY_REACTOR_USE_EPOLL)

# multi thread support
find_package(Threads REQUIRED)

# json support
find_library(JSONCPP_LIB jsoncpp /usr/lib /usr/lib/x86_64-linux-gnu/ /usr/local/lib NO_DEFAULT_PATH)

# 'gcc -I' include directories
include_directories(${PROJECT_SOURCE_DIR} 
    ${PROJECT_SOURCE_DIR}/base 
    ${PROJECT_SOURCE_DIR}/net 
    ${PROJECT_SOURCE_DIR}/util 
    ${PROJECT_SOURCE_DIR}/HttpServer)

# binary output path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin)

# sub directories
add_subdirectory(base)
add_subdirectory(net)
add_subdirectory(test)
add_subdirectory(util)
add_subdirectory(HttpServer)
