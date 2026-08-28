cmake_minimum_required(VERSION 3.16)

project(GeometricMotionPlanning)

# ---------------------------------
# C++ configuration
# ---------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------
# Find SFML
# ---------------------------------
find_package(
    SFML 3
    REQUIRED
    COMPONENTS Graphics Window System
)
# ---------------------------------
# Main GUI application
# ---------------------------------

add_executable(
    planner

    src/main.cpp
    src/geometry/Geometry.cpp
    src/visualization/Renderer.cpp
)

target_include_directories(
    planner
    PRIVATE
    src
)

target_link_libraries(
    planner
    PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)


# ---------------------------------
# Geometry unit tests
# ---------------------------------

add_executable(
    geometry_tests

    tests/geometry_tests.cpp
    src/geometry/Geometry.cpp
)

target_include_directories(
    geometry_tests
    PRIVATE
    src
)