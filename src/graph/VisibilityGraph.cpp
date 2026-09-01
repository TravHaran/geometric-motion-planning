#include "VisibilityGraph.hpp"
#include "../geometry/Geometry.hpp"

#include <cmath>


std::vector<GraphNode> buildVisibilityNodes(
    const Point& start,
    const Point& goal,
    const std::vector<Polygon>& obstacles
){
    std::vector<GraphNode> nodes;

    std::size_t nodeCount = 2;
    for(const Polygon& obstacle : obstacles){
        nodeCount += obstacle.vertices.size();
    }
    nodes.reserve(nodeCount);

    // include start and goal nodes
    nodes.push_back({start});
    nodes.push_back({goal});
    // include every obstacle vertex
    for(const Polygon& obstacle : obstacles){
        for(const Point& vertex : obstacle.vertices){
            nodes.push_back({vertex});
        }
    }
    return nodes;
}

std::vector<GraphEdge> buildVisibilityEdges(
    const std::vector<GraphNode>& nodes,
    const std::vector<Polygon>& obstacles
){
    std::vector<GraphEdge> edges;
    edges.reserve(nodes.size() * 4);

    for(std::size_t i = 0; i < nodes.size(); ++i){

        for(std::size_t j = i + 1; j < nodes.size(); ++j){

            const Point& a = nodes[i].position;
            const Point& b = nodes[j].position;

            if(isVisible(a, b, obstacles)){
                // euclidean distance
                double dx = b.x - a.x;
                double dy = b.y - a.y;

                double weight = std::sqrt(
                    dx * dx +
                    dy * dy
                );

                edges.push_back({i, j, weight});
            }
        }
    }

    return edges;
}

Graph buildVisibilityGraph(
    const Point& start,
    const Point& goal,
    const std::vector<Polygon>& obstacles
){
    Graph graph;
    graph.nodes = buildVisibilityNodes(start, goal, obstacles);
    graph.edges = buildVisibilityEdges(graph.nodes, obstacles);
    return graph;
}
