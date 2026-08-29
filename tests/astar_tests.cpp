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
}

void testAStarUndirectedEdge(){

    Graph graph;

    graph.nodes = {
        {{0.0, 0.0}},
        {{2.5, 0.0}}
    };

    // Deliberately stored backward.
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

    assert(
        approximatelyEqual(
            aStarResult.distance,
            dijkstraResult.distance
        )
    );

    assert(
        !aStarResult.path.empty()
    );
}

int main(){

    testAStarShortestPath();
    testAStarUnreachableGoal();
    testAStarStartEqualsGoal();
    testAStarUndirectedEdge();
    testAStarMatchesDijkstra();

    std::cout
        << "All A* tests passed.\n";

    return 0;
}