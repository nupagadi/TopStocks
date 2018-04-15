project(Display)
cmake_minimum_required(VERSION 3.1)

set(SOURCES
    Display.cpp
)

add_executable(Display ${SOURCES})

target_include_directories(Display PRIVATE .)
