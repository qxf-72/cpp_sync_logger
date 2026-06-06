cmake_minimum_required(VERSION 3.10)

project(cpp_sync_logger)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_executable(sync_logger_demo
    test/main.cpp
    src/Logger.cpp
)

target_include_directories(sync_logger_demo PRIVATE include)

target_link_libraries(sync_logger_demo PRIVATE Threads::Threads)
