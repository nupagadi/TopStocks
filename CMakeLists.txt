project(TopStocks)
cmake_minimum_required(VERSION 3.1)

set(SOURCES
    ITopStocks.hpp
    TopStocks.hpp
)

add_subdirectory(UnitTests)
add_subdirectory(Display)
