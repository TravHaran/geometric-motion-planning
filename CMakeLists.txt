cmake_minimum_required(VERSION 3.16)

project(GeometricMotionPlanning LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(PATHLAB_ENABLE_WARNINGS "Enable compiler warnings for PATHLAB targets" ON)

if(PATHLAB_ENABLE_WARNINGS)
    if(MSVC)
        add_compile_options(/W4)
    else()
        add_compile_options(
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wshadow
        )
    endif()
endif()

find_package(
    SFML 3
    REQUIRED
    COMPONENTS Graphics Window System
)

# ---------------------------------
# Core project libraries
# ---------------------------------

add_library(
    pathlab_geometry
    src/geometry/Geometry.cpp
    src/geometry/Clearance.cpp
    src/geometry/Triangulation.cpp
)

target_include_directories(
    pathlab_geometry
    PUBLIC
    src
)

add_library(
    pathlab_graph
    src/graph/VisibilityGraph.cpp
)

target_include_directories(
    pathlab_graph
    PUBLIC
    src
)

target_link_libraries(
    pathlab_graph
    PUBLIC
    pathlab_geometry
)

add_library(
    pathlab_planners
    src/planners/AStar.cpp
    src/planners/BFS.cpp
    src/planners/Dijkstra.cpp
)

target_include_directories(
    pathlab_planners
    PUBLIC
    src
)

add_library(
    pathlab_visualization
    src/visualization/DemoPanel.cpp
    src/visualization/Renderer.cpp
)

target_include_directories(
    pathlab_visualization
    PUBLIC
    src
)

target_link_libraries(
    pathlab_visualization
    PUBLIC
    SFML::Graphics
    SFML::Window
    SFML::System
)

add_library(
    pathlab_ui
    src/ui/PathlabUI.cpp
    src/ui/PathlabUIInteraction.cpp
    src/ui/PathlabUILayout.cpp
)

target_include_directories(
    pathlab_ui
    PUBLIC
    src
)

target_link_libraries(
    pathlab_ui
    PUBLIC
    SFML::Graphics
    SFML::Window
    SFML::System
)

function(copy_pathlab_assets target)
    add_custom_command(
        TARGET ${target}
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:${target}>/assets"
    )
endfunction()

# ---------------------------------
# Permanent milestone demos
# ---------------------------------

add_executable(
    demo_1a
    demos/demo_1a_segment_intersection.cpp
)

target_link_libraries(
    demo_1a
    PRIVATE
    pathlab_geometry
    pathlab_visualization
)

add_executable(
    demo_1b
    demos/demo_1b_point_in_polygon.cpp
)

target_link_libraries(
    demo_1b
    PRIVATE
    pathlab_geometry
    pathlab_visualization
)

add_executable(
    demo_1c
    demos/demo_1c_path_collision.cpp
)

target_link_libraries(
    demo_1c
    PRIVATE
    pathlab_geometry
    pathlab_visualization
)

add_executable(
    demo_2a
    demos/demo_2a_visibility_graph.cpp
)

target_link_libraries(
    demo_2a
    PRIVATE
    pathlab_graph
    pathlab_visualization
)

add_executable(
    demo_2b
    demos/demo_2b_dijkstra.cpp
)

target_link_libraries(
    demo_2b
    PRIVATE
    pathlab_graph
    pathlab_planners
    pathlab_visualization
)

add_executable(
    demo_2c
    demos/demo_2c_interactive_planner.cpp
)

target_link_libraries(
    demo_2c
    PRIVATE
    pathlab_graph
    pathlab_planners
    pathlab_visualization
)

copy_pathlab_assets(demo_2c)

add_executable(
    demo_2d
    demos/demo_2d_interactive_planner.cpp
)

target_link_libraries(
    demo_2d
    PRIVATE
    pathlab_graph
    pathlab_planners
    pathlab_visualization
)

copy_pathlab_assets(demo_2d)

add_executable(
    demo_2e
    demos/demo_2e_pathlab.cpp
    src/app/PathlabApp.cpp
    src/app/PathlabAppRendering.cpp
)

target_link_libraries(
    demo_2e
    PRIVATE
    pathlab_graph
    pathlab_planners
    pathlab_ui
    pathlab_visualization
)

copy_pathlab_assets(demo_2e)

# ---------------------------------
# Regression tests
# ---------------------------------

enable_testing()

add_executable(
    geometry_tests
    tests/geometry_tests.cpp
)

target_link_libraries(
    geometry_tests
    PRIVATE
    pathlab_geometry
)

add_test(NAME geometry COMMAND geometry_tests)

add_executable(
    graph_tests
    tests/graph_tests.cpp
)

target_link_libraries(
    graph_tests
    PRIVATE
    pathlab_graph
)

add_test(NAME graph COMMAND graph_tests)

add_executable(
    bfs_tests
    tests/bfs_tests.cpp
)

target_link_libraries(
    bfs_tests
    PRIVATE
    pathlab_planners
)

add_test(NAME bfs COMMAND bfs_tests)

add_executable(
    dijkstra_tests
    tests/dijkstra_tests.cpp
)

target_link_libraries(
    dijkstra_tests
    PRIVATE
    pathlab_graph
    pathlab_planners
)

add_test(NAME dijkstra COMMAND dijkstra_tests)

add_executable(
    astar_tests
    tests/astar_tests.cpp
)

target_link_libraries(
    astar_tests
    PRIVATE
    pathlab_graph
    pathlab_planners
)

add_test(NAME astar COMMAND astar_tests)

add_executable(
    pathlab_ui_tests
    tests/pathlab_ui_tests.cpp
)

target_link_libraries(
    pathlab_ui_tests
    PRIVATE
    pathlab_ui
)

add_test(NAME pathlab_ui COMMAND pathlab_ui_tests)

# Phase 3.1 clearance regression tests
add_executable(clearance_tests tests/clearance_tests.cpp)
target_link_libraries(clearance_tests PRIVATE pathlab_geometry)
add_test(NAME clearance COMMAND clearance_tests)

# ---------------------------------
# Hierarchical biped planner prototype
# ---------------------------------
add_library(pathlab_footstep src/footstep/PrototypePlanner.cpp)
target_link_libraries(pathlab_footstep PUBLIC pathlab_geometry)
target_include_directories(pathlab_footstep PUBLIC src)

add_executable(demo_3a demos/demo_3a_footsteps.cpp)
target_link_libraries(demo_3a PRIVATE pathlab_footstep SFML::Graphics SFML::Window SFML::System)
copy_pathlab_assets(demo_3a)

add_executable(footstep_tests tests/footstep_tests.cpp)
target_link_libraries(footstep_tests PRIVATE pathlab_footstep)
add_test(NAME footstep COMMAND footstep_tests)
