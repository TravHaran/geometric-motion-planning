#pragma once
#include <vector>
#include "Point.hpp"
#include "Segment.hpp"
#include "Polygon.hpp"

// Euclidean distances, in the same world units as Point coordinates.
// Valid polygons have >=3 vertices; invalid polygons are ignored by obstacle queries.
double pointSegmentDistance(const Point& p, const Segment& segment);
double segmentSegmentDistance(const Segment& a, const Segment& b);
double pointPolygonDistance(const Point& p, const Polygon& polygon);
double segmentPolygonDistance(const Segment& segment, const Polygon& polygon);

// Clearance is measured from the point robot / centerline to each forbidden polygon.
// Touching the obstacle or the specified clearance boundary is NOT free.
// Clearance must be finite and nonnegative.
bool isPointClearanceFree(const Point& p, const std::vector<Polygon>& obstacles, double clearance);
bool isSegmentClearanceFree(const Segment& segment, const std::vector<Polygon>& obstacles, double clearance);
