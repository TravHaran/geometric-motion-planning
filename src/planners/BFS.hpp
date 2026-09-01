#pragma once

#include <cstddef>
#include <vector>

#include "../graph/Graph.hpp"

struct BFSResult{
    double distance;
    std::vector<std::size_t> path;
    std::size_t expandedNodes;
    std::vector<std::size_t> expandedNodeOrder;
};

BFSResult bfs(
    const Graph& graph,
    std::size_t startNode,
    std::size_t goalNode
);