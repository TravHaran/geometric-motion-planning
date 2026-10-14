#pragma once

#include "Point.hpp"
#include "Segment.hpp"

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
