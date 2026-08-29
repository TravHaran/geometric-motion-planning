#pragma once

#include <cstddef>
#include <vector>

#include "../geometry/Point.hpp"

struct GraphNode{
    Point position;
};

struct GraphEdge{
    std::size_t from;
    std::size_t to;
    double weight;

};

struct Graph{
    std::vector<GraphNode> nodes;
    std::vector<GraphEdge> edges;
};