cmake_minimum_required(VERSION 3.20)
project(RoutePlanner)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/FileParser
        ${CMAKE_SOURCE_DIR}/InputParser
        ${CMAKE_SOURCE_DIR}/RouteFinder
        ${CMAKE_SOURCE_DIR}/OutputWriter
)

# Source files
file(GLOB SOURCES
        "*.cpp"
        "FileParser/*.cpp"
        "InputParser/*.cpp"
        "RouteFinder/*.cpp"
        "OutputWriter/*.cpp"
)

add_executable(RoutePlanner ${SOURCES})