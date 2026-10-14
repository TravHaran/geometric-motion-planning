#pragma once

#include <vector>

#include "Graph.hpp"
#include "../geometry/Point.hpp"
#include "../geometry/Polygon.hpp"

std::vector<GraphNode> buildVisibilityNodes(
    const Point& start,
    const Point& goal,
    const std::vector<Polygon>& obstacles
);

std::vector<GraphEdge> buildVisibilityEdges(
    const std::vector<GraphNode>& nodes,
    const std::vector<Polygon>& obstacles
);

Graph buildVisibilityGraph(
    const Point& start,
    const Point& goal,
    const std::vector<Polygon>& obstacles
);


// Positive clearance uses a conservative, sampled disk-offset waypoint graph.
// The returned edges maintain > clearance distance along their entire segments.
// Graph search optimizes on this finite graph, not continuous free space.
// When clearance == 0, the original visibility graph is returned unchanged.
Graph buildClearanceVisibilityGraph(
    const Point& start,
    const Point& goal,
    const std::vector<Polygon>& obstacles,
    double clearance
);
