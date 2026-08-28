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
# demo 1a
# ---------------------------------

add_executable(
    demo_1a
    demos/demo_1a_segment_intersection.cpp
    src/geometry/Geometry.cpp
    src/visualization/Renderer.cpp
)

target_include_directories(
    demo_1a
    PRIVATE
    src
)

target_link_libraries(
    demo_1a
    PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

# ---------------------------------
# demo 1b
# ---------------------------------
add_executable(
    demo_1b
    demos/demo_1b_point_in_polygon.cpp
    src/geometry/Geometry.cpp
    src/visualization/Renderer.cpp
)

target_include_directories(
    demo_1b
    PRIVATE
    src
)

target_link_libraries(
    demo_1b
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