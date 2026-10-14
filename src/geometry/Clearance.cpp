#include "Clearance.hpp"
#include "Geometry.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

double pointSegmentDistance(const Point& p, const Segment& segment){
    const Point& a = segment.a;
    const Point& b = segment.b;

    // v is the direction from A to B.
    const double vx = b.x - a.x;
    const double vy = b.y - a.y;
    const double lengthSquared = vx * vx + vy * vy;

    // A degenerate segment is just one point.
    if(lengthSquared == 0.0){
        return std::hypot(p.x - a.x, p.y - a.y);
    }

    // Project (P - A) onto the infinite line through A and B.
    const double wx = p.x - a.x;
    const double wy = p.y - a.y;
    const double projectedT = (wx * vx + wy * vy) / lengthSquared;

    // The nearest point on a finite segment must have 0 <= t <= 1.
    const double t = std::clamp(projectedT, 0.0, 1.0);
    const double closestX = a.x + t * vx;
    const double closestY = a.y + t * vy;

    return std::hypot(p.x - closestX, p.y - closestY);
}

double segmentSegmentDistance(const Segment& first, const Segment& second){
    // A common point means the minimum Euclidean distance is zero.
    if(segmentsIntersect(first, second)){
        return 0.0;
    }

    // For disjoint planar segments, a minimum-distance pair can always
    // be chosen such that at least one point is a segment endpoint.
    // Every point-to-segment call already clamps its projection.
    return std::min({
        pointSegmentDistance(first.a, second),
        pointSegmentDistance(first.b, second),
        pointSegmentDistance(second.a, first),
        pointSegmentDistance(second.b, first)
    });
}

double segmentPolygonDistance(const Segment& segment, const Polygon& polygon){
    const std::size_t n = polygon.vertices.size();

    // Match Geometry.cpp: fewer than three vertices is not an obstacle.
    if(n < 3){
        return std::numeric_limits<double>::infinity();
    }

    // A segment contained entirely within a filled polygon can have
    // positive distance to every polygon edge, but is still in collision.
    // The existing pointInPolygon() counts boundary points as inside.
    if(pointInPolygon(segment.a, polygon) ||
       pointInPolygon(segment.b, polygon)){
        return 0.0;
    }

    // Both endpoints may be outside while the segment cuts through the
    // polygon. Each crossing then touches at least one boundary edge,
    // which segmentSegmentDistance() reports as distance zero.
    double minimum = std::numeric_limits<double>::infinity();
    for(std::size_t i = 0; i < n; ++i){
        const Segment boundary{
            polygon.vertices[i],
            polygon.vertices[(i + 1) % n]
        };
        minimum = std::min(minimum, segmentSegmentDistance(segment, boundary));
        if(minimum == 0.0){
            return 0.0;
        }
    }
    return minimum;
}
