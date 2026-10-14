#include "Clearance.hpp"
#include "Geometry.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace {
double squaredDistance(const Point& a, const Point& b) {
    const double dx = a.x - b.x, dy = a.y - b.y;
    return dx * dx + dy * dy;
}
void validateClearance(double clearance) {
    if (!std::isfinite(clearance) || clearance < 0.0)
        throw std::invalid_argument("clearance must be finite and nonnegative");
}
}

double pointSegmentDistance(const Point& p, const Segment& s) {
    const double dx = s.b.x - s.a.x, dy = s.b.y - s.a.y;
    const double length2 = dx * dx + dy * dy;
    if (length2 == 0.0) return std::sqrt(squaredDistance(p, s.a));
    const double t = std::clamp(((p.x - s.a.x)*dx + (p.y - s.a.y)*dy) / length2, 0.0, 1.0);
    return std::hypot(p.x - (s.a.x + t*dx), p.y - (s.a.y + t*dy));
}

double segmentSegmentDistance(const Segment& a, const Segment& b) {
    if (segmentsIntersect(a, b)) return 0.0;
    return std::min({pointSegmentDistance(a.a,b), pointSegmentDistance(a.b,b),
                     pointSegmentDistance(b.a,a), pointSegmentDistance(b.b,a)});
}

double pointPolygonDistance(const Point& p, const Polygon& poly) {
    if (poly.vertices.size() < 3) return std::numeric_limits<double>::infinity();
    if (pointInPolygon(p, poly)) return 0.0;
    double best = std::numeric_limits<double>::infinity();
    for (const auto& edge : polygonEdges(poly))
        best = std::min(best, pointSegmentDistance(p, edge));
    return best;
}

double segmentPolygonDistance(const Segment& s, const Polygon& poly) {
    if (poly.vertices.size() < 3) return std::numeric_limits<double>::infinity();
    if (pointInPolygon(s.a, poly) || pointInPolygon(s.b, poly)) return 0.0;
    double best = std::numeric_limits<double>::infinity();
    for (const auto& edge : polygonEdges(poly)) {
        if (segmentsIntersect(s, edge)) return 0.0;
        best = std::min(best, segmentSegmentDistance(s, edge));
    }
    return best;
}

bool isPointClearanceFree(const Point& p, const std::vector<Polygon>& obstacles, double clearance) {
    validateClearance(clearance);
    for (const auto& poly : obstacles)
        if (pointPolygonDistance(p,poly) <= clearance) return false;
    return true;
}

bool isSegmentClearanceFree(const Segment& s, const std::vector<Polygon>& obstacles, double clearance) {
    validateClearance(clearance);
    for (const auto& poly : obstacles)
        if (segmentPolygonDistance(s,poly) <= clearance) return false;
    return true;
}
