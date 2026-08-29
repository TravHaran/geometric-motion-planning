#include "AStar.hpp"

#include <cmath>
#include <limits>
#include <vector>
#include <algorithm>

namespace { // we want to keep the euclidean heuristic file local
// Euclidean straight-line distance from a node to the goal.
// This is used as A*'s heuristic h(n).
double euclideanHeuristic(
    const GraphNode& node,
    const GraphNode& goal
){
    const double dx =
        goal.position.x
        - node.position.x;

    const double dy =
        goal.position.y
        - node.position.y;

    return std::sqrt(
        dx * dx
        +
        dy * dy
    );
}

// Select the unvisited node with the smallest
//
//     f(n) = g(n) + h(n)
//
// score.
std::size_t findBestUnvisitedNode(
    const Graph& graph,
    const std::vector<double>& distances,
    const std::vector<bool>& visited,
    std::size_t goalNode
){
    std::size_t bestNode = graph.nodes.size();

    double lowestScore = std::numeric_limits<double>::infinity();

    for(std::size_t i = 0; i < graph.nodes.size(); ++i){

        if(visited[i]) continue;

        const double heuristic =
            euclideanHeuristic(
                graph.nodes[i],
                graph.nodes[goalNode]
            );

        const double score = distances[i] + heuristic;

        if(score < lowestScore){
            lowestScore = score;
            bestNode = i;
        }
    }
    return bestNode;
}

}

AStarResult aStar(
    const Graph& graph,
    std::size_t startNode,
    std::size_t goalNode
){
    const std::size_t nodeCount =
        graph.nodes.size();

    // g(n): best known cost from start to each node.
    std::vector<double> distances(
        nodeCount,
        std::numeric_limits<double>::infinity()
    );

    std::vector<bool> visited(
        nodeCount,
        false
    );

    std::vector<std::size_t> parents(
        nodeCount,
        nodeCount
    );

    // The distance from the start node to itself is zero.
    distances[startNode] =
        0.0;

    while(true){

        const std::size_t currentNode =
            findBestUnvisitedNode(
                graph,
                distances,
                visited,
                goalNode
            );

        // No reachable unvisited node remains.
        if(currentNode == nodeCount){
            break;
        }

        // With a consistent heuristic, selecting the goal
        // means its shortest-path cost is finalized.
        if(currentNode == goalNode){
            break;
        }

        visited[currentNode] =
            true;

        // Relax all edges connected to currentNode.
        for(const GraphEdge& edge : graph.edges){

            // nodeCount is used as an invalid-node sentinel.
            std::size_t neighbor =
                nodeCount;

            // The graph is undirected, but each edge
            // is stored only once.
            if(edge.from == currentNode){

                neighbor =
                    edge.to;
            }
            else if(edge.to == currentNode){

                neighbor =
                    edge.from;
            }
            else{

                continue;
            }

            // No need to relax already-finalized nodes.
            if(visited[neighbor]){
                continue;
            }

            const double newDistance =
                distances[currentNode]
                + edge.weight;

            if(newDistance < distances[neighbor]){

                // A shorter route has been found.
                distances[neighbor] =
                    newDistance;

                // Remember how we reached this node.
                parents[neighbor] =
                    currentNode;
            }
        }
    }

    // Reconstruct the shortest path.
    std::vector<std::size_t> path;

    // If the goal distance is still infinity,
    // no path exists from start to goal.
    if(
        distances[goalNode] ==
        std::numeric_limits<double>::infinity()
    ){
        return {
            std::numeric_limits<double>::infinity(),
            path
        };
    }

    // Follow parent links backward from goal to start.
    std::size_t currentNode =
        goalNode;

    while(currentNode != startNode){

        path.push_back(
            currentNode
        );

        currentNode =
            parents[currentNode];
    }

    // Add the start node itself.
    path.push_back(
        startNode
    );

    // Parent traversal produced goal -> start,
    // so reverse it into start -> goal order.
    std::reverse(
        path.begin(),
        path.end()
    );

    return {
        distances[goalNode],
        path
    };
}