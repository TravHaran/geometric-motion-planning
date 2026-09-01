#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

#include "../src/planners/BFS.hpp"
#include "../src/planners/Dijkstra.hpp"


bool approximatelyEqual(
    double a,
    double b,
    double epsilon = 1e-9
){
    return std::abs(a - b) < epsilon;
}


void testBFSShortestHopPath(){

    Graph graph;

    graph.nodes = {
        {{0.0, 0.0}},   // 0: start
        {{4.0, 0.0}},   // 1: goal
        {{1.0, 0.0}},   // 2
        {{2.0, 0.0}},   // 3
        {{3.0, 0.0}}    // 4
    };

    graph.edges = {
        {0, 1, 10.0},   // 1 edge directly to goal

        {0, 2, 1.0},
        {2, 3, 1.0},
        {3, 4, 1.0},
        {4, 1, 1.0}
    };

    BFSResult result =
        bfs(
            graph,
            0,
            1
        );

    /*
     * BFS minimizes number of edges.
     *
     * Therefore it should choose:
     *
     * 0 -> 1
     *
     * even though that path has geometric
     * weight 10.
     */
    assert(
        result.path ==
        std::vector<std::size_t>({
            0,
            1
        })
    );

    assert(
        approximatelyEqual(
            result.distance,
            10.0
        )
    );

    assert(
        result.path.size() - 1 == 1
    );
}


void testBFSUnreachableGoal(){

    Graph graph;

    graph.nodes = {
        {{0.0, 0.0}},
        {{5.0, 0.0}},
        {{2.0, 0.0}}
    };

    graph.edges = {
        {0, 2, 2.0}
    };

    BFSResult result =
        bfs(
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


void testBFSStartEqualsGoal(){

    Graph graph;

    graph.nodes = {
        {{1.0, 2.0}}
    };

    BFSResult result =
        bfs(
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

    /*
     * The start is immediately the goal,
     * so no neighbors are expanded.
     */
    assert(
        result.expandedNodes == 0
    );

    assert(
        result.expandedNodeOrder.empty()
    );
}


void testBFSUndirectedEdge(){

    Graph graph;

    graph.nodes = {
        {{0.0, 0.0}},
        {{2.5, 0.0}}
    };

    /*
     * Deliberately stored backward.
     *
     * BFS should still treat the graph
     * as undirected.
     */
    graph.edges = {
        {1, 0, 2.5}
    };

    BFSResult result =
        bfs(
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
        result.expandedNodes == 1
    );
}


void testBFSExpansionOrder(){

    Graph graph;

    graph.nodes = {
        {{0.0, 0.0}},   // 0
        {{1.0, 0.0}},   // 1
        {{0.0, 1.0}},   // 2
        {{2.0, 0.0}},   // 3
        {{0.0, 2.0}}    // 4
    };

    graph.edges = {
        {0, 1, 1.0},
        {0, 2, 1.0},
        {1, 3, 1.0},
        {2, 4, 1.0}
    };

    BFSResult result =
        bfs(
            graph,
            0,
            4
        );

    /*
     * With adjacency generated in the order
     * above, BFS should process:
     *
     * 0
     * 1
     * 2
     * 3
     *
     * before selecting goal node 4.
     */
    assert(
        result.expandedNodeOrder ==
        std::vector<std::size_t>({
            0,
            1,
            2,
            3
        })
    );

    assert(
        result.expandedNodes ==
        result.expandedNodeOrder.size()
    );
}


void testBFSVsDijkstra(){

    Graph graph;

    graph.nodes = {
        {{0.0, 0.0}},   // 0: start
        {{4.0, 0.0}},   // 1: goal
        {{1.0, 0.0}},   // 2
        {{2.0, 0.0}},   // 3
        {{3.0, 0.0}}    // 4
    };

    graph.edges = {
        /*
         * BFS likes this route because it
         * reaches the goal in one edge.
         */
        {0, 1, 10.0},

        /*
         * Dijkstra prefers this route because
         * its total weighted cost is only 4.
         */
        {0, 2, 1.0},
        {2, 3, 1.0},
        {3, 4, 1.0},
        {4, 1, 1.0}
    };

    BFSResult bfsResult =
        bfs(
            graph,
            0,
            1
        );

    DijkstraResult dijkstraResult =
        dijkstra(
            graph,
            0,
            1
        );

    /*
     * BFS should use fewer edges.
     */
    const std::size_t bfsSegments =
        bfsResult.path.size() - 1;

    const std::size_t dijkstraSegments =
        dijkstraResult.path.size() - 1;

    assert(
        bfsSegments <
        dijkstraSegments
    );

    /*
     * But Dijkstra should produce the
     * shorter weighted / geometric path.
     */
    assert(
        dijkstraResult.distance <
        bfsResult.distance
    );

    assert(
        approximatelyEqual(
            bfsResult.distance,
            10.0
        )
    );

    assert(
        approximatelyEqual(
            dijkstraResult.distance,
            4.0
        )
    );
}


int main(){

    testBFSShortestHopPath();
    testBFSUnreachableGoal();
    testBFSStartEqualsGoal();
    testBFSUndirectedEdge();
    testBFSExpansionOrder();
    testBFSVsDijkstra();

    std::cout
        << "All BFS tests passed.\n";

    return 0;
}