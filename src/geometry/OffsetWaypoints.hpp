#pragma once

#include "Point.hpp"
#include "Polygon.hpp"

#include <vector>

// Generate clearance-safe candidate waypoints for a point robot navigating
// around the union of filled, simple polygon obstacles.
//
// The offset boundary of a polygon contains translated edges, circular arcs
// around convex corners, and intersections at reflex corners. This routine
// samples the convex arcs and proposes offset-line intersections at reflex
// corners; every candidate is checked against ALL input obstacles.
//
// r == 0 returns no candidates: the legacy zero-radius visibility graph
// continues to use the original polygon vertices.
// Contact at exactly r is forbidden, consistent with segmentHasClearance().
//
// These are CANDIDATE VERTICES, not a polygonal Minkowski-sum boundary:
// joining consecutive waypoints may violate clearance. The next milestone
// will check EVERY connecting edge with segmentHasClearance().
//
// A finite candidate set is an approximation, with no global shortest-path
// or completeness guarantee in continuous space.
std::vector<Point> generateClearanceWaypoints(
    const std::vector<Polygon>& obstacles,
    double radius
);
