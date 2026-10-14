#pragma once
#include "Point.hpp"
#include "Segment.hpp"
#include "Polygon.hpp"
#include <vector>

// Euclidean distances to CLOSED segments/polygon regions in PATHLAB world units.
// For polygons, the distance is zero inside or on the obstacle.
double pointSegmentDistance(const Point& point, const Segment& segment);
double segmentSegmentDistance(const Segment& first, const Segment& second);
double pointPolygonDistance(const Point& point, const Polygon& polygon);
double segmentPolygonDistance(const Segment& segment, const Polygon& polygon);

// Collision-free means distance STRICTLY GREATER than the selected radius.
// A zero-radius query forbids contact with obstacles as well.
// Invalid polygons (< 3 vertices) follow existing Geometry.cpp behavior: ignored.
// Throws invalid_argument for a negative, infinite, or NaN clearance.
bool pointHasClearance(const Point& point, const std::vector<Polygon>& obstacles,
                       double radius);
bool segmentHasClearance(const Segment& segment, const std::vector<Polygon>& obstacles,
                         double radius);
