#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

#include "../src/planners/Dijkstra.hpp"
#include "../src/graph/VisibilityGraph.hpp"

bool approximatelyEqual(
    double a,
    double b,
    double epsilon = 1e-9
){
    return std::abs(a - b) < epsilon;
}

void testDijkstraShortestPath(){

    Graph graph;

    graph.nodes = {
        {{0.0, 0.0}},
        {{7.0, 0.0}},
        {{4.0, 0.0}},
        {{2.0, 0.0}}
    };

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

    assert(
        approximatelyEqual(
            result.distance,
            5.0
        )
    );

    assert(
        result.path ==
        std::vector<std::size_t>({
            0,
            3,
            1
        })
    );

    // At least one node should have been expanded
    // before the goal was selected.
    assert(
        result.expandedNodes > 0
    );
}

void testDijkstraUnreachableGoal(){

    Graph graph;

    graph.nodes = {
        {{0.0, 0.0}},
        {{5.0, 0.0}},
        {{2.0, 0.0}}
    };

    graph.edges = {
        {0, 2, 2.0}
    };

    DijkstraResult result =
        dijkstra(
            graph,
            0,
            1
        );

    assert(
        std::isinf(
            result.distance
        )
    );

    assert(
        result.path.empty()
    );

    // Reachable nodes are still expanded while
    // searching for the unreachable goal.
    assert(
        result.expandedNodes > 0
    );
}

void testDijkstraStartEqualsGoal(){

    Graph graph;

    graph.nodes = {
        {{1.0, 2.0}}
    };

    DijkstraResult result =
        dijkstra(
            graph,
            0,
            0
        );

    assert(
        approximatelyEqual(
            result.distance,
            0.0
        )
    );

    assert(
        result.path ==
        std::vector<std::size_t>({
            0
        })
    );

    // The start is immediately recognized as the goal,
    // so no node's edges are expanded.
    assert(
        result.expandedNodes == 0
    );
}

void testDijkstraUndirectedEdge(){

    Graph graph;

    graph.nodes = {
        {{0.0, 0.0}},
        {{2.5, 0.0}}
    };

    // Deliberately store the undirected edge backward.
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
        approximatelyEqual(
            result.distance,
            2.5
        )
    );

    assert(
        result.path ==
        std::vector<std::size_t>({
            0,
            1
        })
    );

    assert(
        result.expandedNodes > 0
    );
}

void testDijkstraVisibilityGraph(){

    const Point start{
        100.0,
        300.0
    };

    const Point goal{
        700.0,
        300.0
    };

    Polygon obstacle{
        {
            {300.0, 200.0},
            {500.0, 200.0},
            {500.0, 400.0},
            {300.0, 400.0}
        }
    };

    std::vector<Polygon> obstacles{
        obstacle
    };

    Graph graph =
        buildVisibilityGraph(
            start,
            goal,
            obstacles
        );

    DijkstraResult result =
        dijkstra(
            graph,
            0,
            1
        );

    assert(
        !std::isinf(
            result.distance
        )
    );

    assert(
        !result.path.empty()
    );

    assert(
        result.path.front() == 0
    );

    assert(
        result.path.back() == 1
    );

    assert(
        result.expandedNodes > 0
    );
}

int main(){

    testDijkstraShortestPath();
    testDijkstraUnreachableGoal();
    testDijkstraStartEqualsGoal();
    testDijkstraUndirectedEdge();
    testDijkstraVisibilityGraph();

    std::cout
        << "All Dijkstra tests passed.\n";

    return 0;
}