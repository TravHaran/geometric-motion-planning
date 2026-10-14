#pragma once

#include "Point.hpp"
#include "Segment.hpp"

// Returns the shortest Euclidean distance from p to the CLOSED segment AB.
// If A and B coincide, the segment is treated as the single point A.
//
// Time: O(1)     Auxiliary space: O(1)
// All coordinates and distances use PATHLAB world units.
double pointSegmentDistance(const Point& p, const Segment& segment);
