#include "BFS.hpp"

#include <algorithm>
#include <limits>
#include <vector>
#include <queue>

namespace{
struct Neighbor{
    std::size_t node;
    double weight;
};

std::vector<std::vector<Neighbor>> buildAdjacencyList(const Graph& graph){
    std::vector<std::vector<Neighbor>> adjacency(graph.nodes.size());

    for(const GraphEdge& edge : graph.edges){
        adjacency[edge.from].push_back({edge.to, edge.weight});
        adjacency[edge.to].push_back({edge.from, edge.weight});
    }

    return adjacency;
}

}

BFSResult bfs(
    const Graph& graph,
    std::size_t startNode,
    std::size_t goalNode
){
    std::size_t expandedNodes = 0;
    std::vector<std::size_t> expandedNodeOrder;

    const std::size_t nodeCount = graph.nodes.size();

    const std::vector<std::vector<Neighbor>> adjacency = buildAdjacencyList(graph);

    std::vector<bool> visited(nodeCount, false);

    std::vector<std::size_t> parents(nodeCount, nodeCount);

    /*
     * Stores the weight of the edge used
     * to first reach each node.
     *
     * BFS does NOT use these weights during
     * search. They are retained only so that
     * we can report the geometric length of
     * the final BFS path afterward.
     */

    std::vector<double> parentEdgeWeights(nodeCount, 0.0);

    std::queue<std::size_t> frontier;

    // initialize search
    frontier.push(startNode);

    visited[startNode] = true;

    // bfs
    while(!frontier.empty()){
        const std::size_t currentNode = frontier.front();

        frontier.pop();

        // goal selected
        if(currentNode == goalNode) break;
        ++expandedNodes;

        expandedNodeOrder.push_back(currentNode);

        // explore every neighbor
        for(const Neighbor& neighbor : adjacency[currentNode]){
            if(visited[neighbor.node]) continue;

            visited[neighbor.node] = true;

            parents[neighbor.node] = currentNode;

            parentEdgeWeights[neighbor.node] = neighbor.weight;

            frontier.push(neighbor.node);
        }
    }

    // path reconstruction

    std::vector<std::size_t> path;

    // if goal not discovered
    if(!visited[goalNode]){
        return {
            std::numeric_limits<double>::infinity(),
            path,
            expandedNodes,
            expandedNodeOrder
        };
    }

    double pathDistance = 0.0;

    std::size_t currentNode = goalNode;

    // follow parent links backward from goal to start
    while(currentNode != startNode){
        path.push_back(currentNode);

        pathDistance += parentEdgeWeights[currentNode];

        currentNode = parents[currentNode];
    }

    // add start node itself
    path.push_back(startNode);

    std::reverse(path.begin(), path.end());

    return {
        pathDistance,
        path,
        expandedNodes,
        expandedNodeOrder
    };
}