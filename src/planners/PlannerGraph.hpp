#pragma once

#include <cstddef>
#include <vector>

#include "../graph/Graph.hpp"

namespace planner_detail{

struct Neighbor{
    std::size_t node;
    double weight;
};

using AdjacencyList =
    std::vector<std::vector<Neighbor>>;

inline AdjacencyList buildAdjacencyList(const Graph& graph){
    AdjacencyList adjacency(graph.nodes.size());

    // Graph edges are stored once even though the graph is undirected.
    for(const GraphEdge& edge : graph.edges){
        adjacency[edge.from].push_back({edge.to, edge.weight});
        adjacency[edge.to].push_back({edge.from, edge.weight});
    }

    return adjacency;
}

}
