#pragma once

#include "Point.hpp"
#include "Segment.hpp"
#include "Polygon.hpp"
#include <vector>

// Returns the shortest Euclidean distance from p to the CLOSED segment AB.
// If A and B coincide, the segment is treated as the single point A.
//
// Time: O(1)     Auxiliary space: O(1)
// All coordinates and distances use PATHLAB world units.
double pointSegmentDistance(const Point& p, const Segment& segment);

// Returns the minimum Euclidean distance between two CLOSED segments.
// Touching, crossing, and overlapping segments have distance zero.
// Uses the existing PATHLAB intersection predicate and the
// point-to-segment primitive. Time O(1), auxiliary space O(1).
double segmentSegmentDistance(const Segment& first, const Segment& second);

// Minimum distance from the CLOSED segment to the FILLED simple polygon.
// Returns zero when the segment intersects, touches, or lies within the polygon.
// Supports convex and concave polygons in either winding direction.
// Polygons with fewer than three vertices are treated as absent (+infinity),
// matching the existing PATHLAB invalid-polygon convention.
// Time: O(n) for n polygon vertices; auxiliary space: O(1).
double segmentPolygonDistance(const Segment& segment, const Polygon& polygon);

// Returns true only when EVERY point on the closed candidate segment is
// STRICTLY farther than radius from EVERY filled polygonal obstacle.
// Therefore contact with an obstacle or with its clearance boundary is blocked,
// including at radius == 0. Invalid polygons (<3 vertices) are ignored.
// radius must be finite and nonnegative; throws std::invalid_argument otherwise.
// Assumes finite segment and polygon vertex coordinates.
//
// Time: O(sum of obstacle vertex counts), space: O(1).
bool segmentHasClearance(
    const Segment& segment,
    const std::vector<Polygon>& obstacles,
    double radius
);
