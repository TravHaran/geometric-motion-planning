#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

#include "../src/graph/VisibilityGraph.hpp"

void testBuildVisibilityNodes()
{
    Point start{0.0, 0.0};
    Point goal{10.0, 10.0};

    Polygon obstacle{{
        {2.0, 2.0},
        {4.0, 2.0},
        {4.0, 4.0},
        {2.0, 4.0}
    }};

    std::vector<Polygon> obstacles{
        obstacle
    };

    std::vector<GraphNode> nodes =
        buildVisibilityNodes(start, goal, obstacles);

    // Start + goal + 4 obstacle vertices = 6 nodes.
    assert(nodes.size() == 6);

    // Start is node 0.
    assert(nodes[0].position.x == 0.0);
    assert(nodes[0].position.y == 0.0);

    // Goal is node 1.
    assert(nodes[1].position.x == 10.0);
    assert(nodes[1].position.y == 10.0);

    // Obstacle vertices follow in their original order.
    assert(nodes[2].position.x == 2.0);
    assert(nodes[2].position.y == 2.0);

    assert(nodes[3].position.x == 4.0);
    assert(nodes[3].position.y == 2.0);

    assert(nodes[4].position.x == 4.0);
    assert(nodes[4].position.y == 4.0);

    assert(nodes[5].position.x == 2.0);
    assert(nodes[5].position.y == 4.0);
}

void testBuildVisibilityEdges()
{
    Polygon obstacle{{
        {2.0, 2.0},
        {4.0, 2.0},
        {4.0, 4.0},
        {2.0, 4.0}
    }};

    std::vector<Polygon> obstacles{
        obstacle
    };

    std::vector<GraphNode> nodes{
        {{0.0, 3.0}},   // Node 0
        {{6.0, 3.0}},   // Node 1
        {{0.0, 6.0}}    // Node 2
    };

    std::vector<GraphEdge> edges =
        buildVisibilityEdges(nodes, obstacles);

    // Node 0 -> Node 1 passes directly through the obstacle,
    // so that edge should not exist.
    //
    // Node 0 -> Node 2 is completely outside the obstacle.
    //
    // Node 1 -> Node 2 touches the obstacle at the top-right
    // vertex but does not enter its interior, so it is visible.
    assert(edges.size() == 2);

    // First visible pair should be (0, 2).
    assert(edges[0].from == 0);
    assert(edges[0].to == 2);

    // Distance from (0,3) to (0,6) = 3.
    assert(std::abs(edges[0].weight - 3.0) < 1e-9);

    // Second visible pair should be (1, 2).
    assert(edges[1].from == 1);
    assert(edges[1].to == 2);

    // Distance:
    //
    // sqrt((0 - 6)^2 + (6 - 3)^2)
    // = sqrt(36 + 9)
    // = sqrt(45)
    double expectedWeight = std::sqrt(45.0);

    assert(
        std::abs(edges[1].weight - expectedWeight)
        < 1e-9
    );
}

void testBuildVisibilityGraph()
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

    Graph graph =
        buildVisibilityGraph(
            start,
            goal,
            obstacles
        );

    // Start + goal + 4 obstacle vertices.
    assert(graph.nodes.size() == 6);

    // Start and goal remain the first two nodes.
    assert(graph.nodes[0].position.x == 0.0);
    assert(graph.nodes[0].position.y == 3.0);

    assert(graph.nodes[1].position.x == 6.0);
    assert(graph.nodes[1].position.y == 3.0);

    // The direct start-to-goal segment passes through
    // the obstacle, so there must not be an edge (0, 1).
    bool directEdgeExists = false;

    for(const GraphEdge& edge : graph.edges){
        if(edge.from == 0 && edge.to == 1){
            directEdgeExists = true;
        }
    }

    assert(!directEdgeExists);

    // A nontrivial visibility graph should contain
    // some valid edges around the obstacle.
    assert(!graph.edges.empty());
}

int main()
{
    testBuildVisibilityNodes();
    testBuildVisibilityEdges();
    testBuildVisibilityGraph();

    std::cout << "All graph tests passed!\n";

    return 0;
}