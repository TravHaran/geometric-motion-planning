#pragma once
#include "Graph.hpp"
#include "../geometry/Polygon.hpp"
#include <vector>

// Approximate clearance-aware visibility graph over validated offset samples.
// At radius zero, delegates to the unchanged classical visibility graph.
// For positive radius all edges pass continuous segment-to-polygon checks;
// no optimality/completeness guarantee between sampled waypoints.
// Node 0 = start, node 1 = goal. Invalid endpoints remain disconnected.
Graph buildClearanceVisibilityGraph(
    const Point& start, const Point& goal,
    const std::vector<Polygon>& obstacles, double radius
);
