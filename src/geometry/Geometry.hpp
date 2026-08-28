#pragma once

#include <vector>

#include "Point.hpp"
#include "Segment.hpp"
#include "Polygon.hpp"
#include "Path.hpp"

double orientation(
    const Point& a,
    const Point& b, 
    const Point& c
);

bool onSegment(
    const Point& a,
    const Point& b, 
    const Point& p
);

bool segmentsIntersect(
    const Segment& s1,
    const Segment& s2
);

std::vector<Segment> polygonEdges(const Polygon& polygon);

bool segmentIntersectsPolygon(const Segment& segment, const Polygon& polygon);

bool pointInPolygon(
    const Point& point,
    const Polygon& polygon
);

bool isSegmentCollisionFree(
    const Segment& segment,
    const std::vector<Polygon>& obstacles
);

bool isPathCollisionFree(
    const Path& path,
    const std::vector<Polygon>& obstacles
);