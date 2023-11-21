cmake_minimum_required(VERSION 3.12)

project(RoboVis)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

find_package(raylib REQUIRED)

set(SOURCES
    src/main.cpp
    src/Visualizer.cpp
)

include_directories(src)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
