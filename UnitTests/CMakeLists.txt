project(UnitTests)
cmake_minimum_required(VERSION 3.1)

set(SOURCES
    TopStocksHandlerMock.hpp
    UnitTests.cpp
)

add_executable(UnitTests ${SOURCES})

target_include_directories(UnitTests PRIVATE .)
