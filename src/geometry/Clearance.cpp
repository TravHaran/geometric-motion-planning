#include "Clearance.hpp"
#include "Geometry.hpp"

#include <algorithm>
#include <cmath>

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
