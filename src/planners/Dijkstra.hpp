#pragma once

#include <cstddef>
#include <vector>

#include "../graph/Graph.hpp"


struct DijkstraResult{
    double distance;
    std::vector<std::size_t> path;
};

DijkstraResult dijkstra(
    const Graph& graph,
    std::size_t startNode,
    std::size_t goalNode
);