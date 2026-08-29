#pragma once

#include <cstddef>
#include <vector>

#include "../graph/Graph.hpp"

struct AStarResult{
    double distance;
    std::vector<std::size_t> path;
};

AStarResult aStar(
    const Graph& graph,
    std::size_t startNode,
    std::size_t goalNode
);