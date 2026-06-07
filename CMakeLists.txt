cmake_minimum_required(VERSION 3.10)

project(cpp_sync_logger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_library(sync_logger STATIC
    src/Logger.cpp
)

target_include_directories(sync_logger
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(sync_logger
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

target_link_libraries(sync_logger
    PUBLIC
        Threads::Threads
)

add_executable(sync_logger_demo
    test/main.cpp
)

target_link_libraries(sync_logger_demo
    PRIVATE
        sync_logger
)
