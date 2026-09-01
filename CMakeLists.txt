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
# demo 1c
# ---------------------------------
add_executable(
    demo_1c

    demos/demo_1c_path_collision.cpp
    src/geometry/Geometry.cpp
    src/visualization/Renderer.cpp
)

target_include_directories(
    demo_1c
    PRIVATE
    src
)

target_link_libraries(
    demo_1c
    PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

# ---------------------------------
# demo 2a
# ---------------------------------

add_executable(
    demo_2a
    demos/demo_2a_visibility_graph.cpp
    src/graph/VisibilityGraph.cpp
    src/geometry/Geometry.cpp
    src/visualization/Renderer.cpp
)

target_include_directories(
    demo_2a
    PRIVATE
    src
)

target_link_libraries(
    demo_2a
    PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)


# ---------------------------------
# demo 2b
# ---------------------------------

add_executable(
    demo_2b

    demos/demo_2b_dijkstra.cpp
    src/graph/VisibilityGraph.cpp
    src/geometry/Geometry.cpp
    src/visualization/Renderer.cpp
    src/planners/Dijkstra.cpp
)

target_include_directories(
    demo_2b
    PRIVATE
    src
)

target_link_libraries(
    demo_2b
    PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

# ---------------------------------
# demo 2c
# ---------------------------------

add_executable(
    demo_2c

    demos/demo_2c_interactive_planner.cpp
    src/geometry/Geometry.cpp
    src/visualization/Renderer.cpp
    src/graph/VisibilityGraph.cpp
    src/planners/Dijkstra.cpp
    src/visualization/DemoPanel.cpp
)

target_include_directories(
    demo_2c
    PRIVATE
    src
)

target_link_libraries(
    demo_2c
    PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

add_custom_command(
    TARGET demo_2c
    POST_BUILD

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:demo_2c>/assets"
)

# ---------------------------------
# demo 2d - interactive Dijkstra/A*
# ---------------------------------

add_executable(
    demo_2d

    demos/demo_2d_interactive_planner.cpp

    src/geometry/Geometry.cpp

    src/graph/VisibilityGraph.cpp

    src/planners/Dijkstra.cpp
    src/planners/AStar.cpp

    src/visualization/Renderer.cpp
    src/visualization/DemoPanel.cpp
)

target_include_directories(
    demo_2d
    PRIVATE
    src
)

target_link_libraries(
    demo_2d
    PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

add_custom_command(
    TARGET demo_2d
    POST_BUILD

    COMMAND
        ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:demo_2d>/assets
)

# ---------------------------------
# demo 2e - PATHLAB
# ---------------------------------

add_executable(
    demo_2e

    demos/demo_2e_pathlab.cpp

    src/app/PathlabApp.cpp

    src/geometry/Geometry.cpp

    src/geometry/Triangulation.cpp

    src/graph/VisibilityGraph.cpp

    src/planners/Dijkstra.cpp

    src/planners/AStar.cpp

    src/visualization/Renderer.cpp

    src/ui/PathlabUI.cpp
)

target_include_directories(
    demo_2e
    PRIVATE
    src
)

target_link_libraries(
    demo_2e
    PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

add_custom_command(
    TARGET demo_2e
    POST_BUILD

    COMMAND
        ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:demo_2e>/assets
)

# ---------------------------------
# Geometry unit tests
# ---------------------------------

add_executable(
    geometry_tests

    tests/geometry_tests.cpp
    src/geometry/Geometry.cpp
    src/geometry/Triangulation.cpp
)

target_include_directories(
    geometry_tests
    PRIVATE
    src
)

# ---------------------------------
# Graph unit tests
# ---------------------------------

add_executable(
    graph_tests

    tests/graph_tests.cpp
    src/graph/VisibilityGraph.cpp
    src/geometry/Geometry.cpp
)

target_include_directories(
    graph_tests
    PRIVATE
    src
)

# ---------------------------------
# Dijkstra unit tests
# ---------------------------------

add_executable(
    dijkstra_tests

    tests/dijkstra_tests.cpp
    src/planners/Dijkstra.cpp
    src/graph/VisibilityGraph.cpp
    src/geometry/Geometry.cpp
)

target_include_directories(
    dijkstra_tests
    PRIVATE
    src
)

# ---------------------------------
# AStar unit tests
# ---------------------------------

add_executable(
    astar_tests

    tests/astar_tests.cpp
    src/planners/AStar.cpp
    src/planners/Dijkstra.cpp
    src/graph/VisibilityGraph.cpp
    src/geometry/Geometry.cpp
)

target_include_directories(
    astar_tests
    PRIVATE
    src
)
