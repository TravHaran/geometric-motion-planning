#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

#include "../src/planners/AStar.hpp"
#include "../src/planners/Dijkstra.hpp"
#include "../src/graph/VisibilityGraph.hpp"

bool approximatelyEqual(
    double a,
    double b,
    double epsilon = 1e-9
){
    return std::abs(a - b) < epsilon;
}

void testAStarShortestPath(){

    Graph graph;

    graph.nodes = {
        {{0.0, 0.0}},
        {{6.0, 0.0}},
        {{2.0, 0.0}},
        {{3.0, 4.0}}
    };

    graph.edges = {
        {0, 1, 10.0},
        {0, 2, 2.0},
        {2, 1, 4.0},
        {0, 3, 5.0},
        {3, 1, 5.0}
    };

    AStarResult result =
        aStar(
            graph,
            0,
            1
        );

    assert(
        approximatelyEqual(
            result.distance,
            6.0
        )
    );

    assert(
        result.path ==
        std::vector<std::size_t>({
            0,
            2,
            1
        })
    );

    assert(
        result.expandedNodes > 0
    );
}

void testAStarUnreachableGoal(){

    Graph graph;

    graph.nodes = {
        {{0.0, 0.0}},
        {{5.0, 0.0}},
        {{2.0, 0.0}}
    };

    graph.edges = {
        {0, 2, 2.0}
    };

    AStarResult result =
        aStar(
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

    assert(
        result.expandedNodes > 0
    );
}

void testAStarStartEqualsGoal(){

    Graph graph;

    graph.nodes = {
        {{1.0, 2.0}}
    };

    AStarResult result =
        aStar(
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

    // The goal is selected immediately,
    // so its outgoing edges are never expanded.
    assert(
        result.expandedNodes == 0
    );
}

void testAStarUndirectedEdge(){

    Graph graph;

    graph.nodes = {
        {{0.0, 0.0}},
        {{2.5, 0.0}}
    };

    // Deliberately store the edge backward.
    graph.edges = {
        {1, 0, 2.5}
    };

    AStarResult result =
        aStar(
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

void testAStarMatchesDijkstra(){

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

    DijkstraResult dijkstraResult =
        dijkstra(
            graph,
            0,
            1
        );

    AStarResult aStarResult =
        aStar(
            graph,
            0,
            1
        );

    // Both algorithms should find the same
    // optimal shortest-path distance.
    assert(
        approximatelyEqual(
            aStarResult.distance,
            dijkstraResult.distance
        )
    );

    assert(
        !aStarResult.path.empty()
    );

    assert(
        aStarResult.path.front() == 0
    );

    assert(
        aStarResult.path.back() == 1
    );

    assert(
        aStarResult.expandedNodes > 0
    );

    assert(
        dijkstraResult.expandedNodes > 0
    );
}

void testAStarExpandsFewerNodesThanDijkstra(){

    Graph graph;

    graph.nodes = {
        {{0.0, 0.0}},   // 0: start
        {{10.0, 0.0}},  // 1: goal
        {{5.0, 0.0}},   // 2: useful intermediate node
        {{0.0, 1.0}},   // 3: distractor
        {{0.0, 2.0}},   // 4: distractor
        {{0.0, 3.0}}    // 5: distractor
    };

    graph.edges = {
        {0, 2, 5.0},
        {2, 1, 5.0},

        {0, 3, 1.0},
        {3, 4, 1.0},
        {4, 5, 1.0}
    };

    DijkstraResult dijkstraResult =
        dijkstra(
            graph,
            0,
            1
        );

    AStarResult aStarResult =
        aStar(
            graph,
            0,
            1
        );

    // Both algorithms must still find
    // the same optimal path cost.
    assert(
        approximatelyEqual(
            aStarResult.distance,
            dijkstraResult.distance
        )
    );

    assert(
        approximatelyEqual(
            aStarResult.distance,
            10.0
        )
    );

    // A* should avoid expanding the
    // irrelevant branch.
    assert(
        aStarResult.expandedNodes <
        dijkstraResult.expandedNodes
    );
}

int main(){

    testAStarShortestPath();
    testAStarUnreachableGoal();
    testAStarStartEqualsGoal();
    testAStarUndirectedEdge();
    testAStarMatchesDijkstra();
    testAStarExpandsFewerNodesThanDijkstra();

    std::cout
        << "All A* tests passed.\n";

    return 0;
}