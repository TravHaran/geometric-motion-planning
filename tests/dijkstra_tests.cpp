#include <cassert>
#include <cmath>
#include <iostream>

#include "../src/planners/Dijkstra.hpp"
#include "../src/graph/VisibilityGraph.hpp"


void testDijkstraShortestPath()
{
    Graph graph;

    // Dijkstra only needs the graph connectivity and weights
    // for this test, but create four nodes so indices 0-3 exist.
    graph.nodes = {
        {{0.0, 0.0}},   // Node 0
        {{1.0, 0.0}},   // Node 1
        {{0.0, 1.0}},   // Node 2
        {{1.0, 1.0}}    // Node 3
    };

    /*
     * Weighted graph:
     *
     *         2
     *       /   \
     *     4       2
     *    /         \
     *   0 ----7---- 1
     *    \         /
     *     2       3
     *       \   /
     *         3
     */
    graph.edges = {
        {0, 1, 7.0},
        {0, 2, 4.0},
        {0, 3, 2.0},
        {2, 1, 2.0},
        {3, 1, 3.0}
    };

    DijkstraResult result =
        dijkstra(
            graph,
            0,
            1
        );

    // Expected shortest path:
    //
    // 0 -> 3 -> 1
    //
    // Cost = 2 + 3 = 5
    assert(
        std::abs(result.distance - 5.0)
        < 1e-9
    );

    assert(result.path.size() == 3);

    assert(result.path[0] == 0);
    assert(result.path[1] == 3);
    assert(result.path[2] == 1);
}

void testDijkstraUnreachableGoal()
{
    Graph graph;

    graph.nodes = {
        {{0.0, 0.0}},
        {{1.0, 0.0}},
        {{2.0, 0.0}}
    };

    // Node 2 is disconnected.
    graph.edges = {
        {0, 1, 1.0}
    };

    DijkstraResult result =
        dijkstra(
            graph,
            0,
            2
        );

    assert(std::isinf(result.distance));
    assert(result.path.empty());
}

void testDijkstraStartEqualsGoal()
{
    Graph graph;

    graph.nodes = {
        {{0.0, 0.0}},
        {{1.0, 0.0}}
    };

    graph.edges = {
        {0, 1, 5.0}
    };

    DijkstraResult result =
        dijkstra(
            graph,
            0,
            0
        );

    assert(
        std::abs(result.distance - 0.0)
        < 1e-9
    );

    assert(result.path.size() == 1);
    assert(result.path[0] == 0);
}

void testDijkstraOnVisibilityGraph()
{
    Point start{0.0, 3.0};
    Point goal{6.0, 3.0};

    Polygon obstacle{{
        {2.0, 2.0},
        {4.0, 2.0},
        {4.0, 4.0},
        {2.0, 4.0}
    }};

    std::vector<Polygon> obstacles{
        obstacle
    };

    // Build the visibility graph directly from the geometry.
    Graph graph =
        buildVisibilityGraph(
            start,
            goal,
            obstacles
        );

    /*
     * buildVisibilityGraph() guarantees:
     *
     * node 0 = start
     * node 1 = goal
     */
    DijkstraResult result =
        dijkstra(
            graph,
            0,
            1
        );

    // A path around the obstacle should exist.
    assert(!result.path.empty());

    // The path must begin at the start node.
    assert(result.path.front() == 0);

    // The path must end at the goal node.
    assert(result.path.back() == 1);

    // The direct start -> goal path is blocked,
    // so at least one obstacle vertex must be used.
    assert(result.path.size() > 2);

    // The straight-line start-to-goal distance is 6,
    // so going around the obstacle must be longer.
    assert(result.distance > 6.0);
}

void testDijkstraUndirectedEdge()
{
    Graph graph;

    graph.nodes = {
        {{0.0, 0.0}},
        {{1.0, 0.0}}
    };

    // Stored as 1 -> 0, but we will search from 0 -> 1.
    graph.edges = {
        {1, 0, 2.5}
    };

    DijkstraResult result =
        dijkstra(
            graph,
            0,
            1
        );

    assert(
        std::abs(result.distance - 2.5)
        < 1e-9
    );

    assert(result.path.size() == 2);
    assert(result.path[0] == 0);
    assert(result.path[1] == 1);
}


int main()
{
    testDijkstraShortestPath();
    testDijkstraUnreachableGoal();
    testDijkstraStartEqualsGoal();
    testDijkstraOnVisibilityGraph();
    testDijkstraUndirectedEdge();

    std::cout
        << "All Dijkstra tests passed!\n";

    return 0;
}