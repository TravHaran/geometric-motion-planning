#include "geometry/Clearance.hpp"

#include <cmath>
#include <iostream>
#include <limits>
#include <vector>
#include <stdexcept>
#include <string>

namespace {
void expectDistance(const std::string& caseName,
                    const Point& p, const Segment& segment,
                    double expected){
    const double actual = pointSegmentDistance(p, segment);
    if(!std::isfinite(actual) || std::abs(actual - expected) > 1e-10){
        throw std::runtime_error(caseName + ": incorrect point-to-segment distance");
    }
}
void expectSegmentDistance(const std::string& caseName,
                           const Segment& first, const Segment& second,
                           double expected){
    const double actual=segmentSegmentDistance(first,second);
    const double reversed=segmentSegmentDistance(second,first);
    if(!std::isfinite(actual) || !std::isfinite(reversed) ||
       std::abs(actual-expected)>1e-10 ||
       std::abs(reversed-expected)>1e-10){
        throw std::runtime_error(caseName + ": incorrect segment-to-segment distance");
    }
}
void expectPolygonDistance(const std::string& caseName,
                           const Segment& segment,
                           const Polygon& polygon,
                           double expected){
    const double actual = segmentPolygonDistance(segment, polygon);
    const Segment reversed{segment.b, segment.a};
    const double reverseActual = segmentPolygonDistance(reversed, polygon);

    if(!std::isfinite(actual) || !std::isfinite(reverseActual) ||
       std::abs(actual - expected) > 1e-10 ||
       std::abs(reverseActual - expected) > 1e-10){
        throw std::runtime_error(caseName + ": incorrect segment-to-polygon distance");
    }
}
void expectClearance(const std::string& name,
                     const Segment& candidate,
                     const std::vector<Polygon>& obstacles,
                     double radius,
                     bool expected){
    const bool actual = segmentHasClearance(candidate, obstacles, radius);
    const Segment backwards{candidate.b,candidate.a};
    const bool reversed = segmentHasClearance(backwards, obstacles, radius);
    if(actual != expected || reversed != expected){
        throw std::runtime_error(name + ": clearance classification incorrect");
    }
}
void expectInvalidRadius(const std::string& name, double radius){
    bool threw = false;
    try{
        (void)segmentHasClearance({{0,0},{1,0}}, {}, radius);
    }catch(const std::invalid_argument&){
        threw = true;
    }
    if(!threw) throw std::runtime_error(name + ": expected invalid_argument");
}
}

int main(){
    // Interior perpendicular projection (t = 0.5).
    expectDistance("interior projection", {2,3}, {{0,0},{4,0}}, 3.0);

    // The perpendicular projection falls before A (t < 0).
    expectDistance("before A", {-3,4}, {{0,0},{4,0}}, 5.0);

    // The perpendicular projection falls after B (t > 1).
    expectDistance("after B", {7,4}, {{0,0},{4,0}}, 5.0);

    // Point exactly on the segment and at an endpoint.
    expectDistance("on segment", {2,0}, {{0,0},{4,0}}, 0.0);
    expectDistance("at endpoint", {4,0}, {{0,0},{4,0}}, 0.0);

    // Degenerate segment (A = B).
    expectDistance("zero length", {3,4}, {{0,0},{0,0}}, 5.0);

    // Vertical, diagonal, and reversed endpoint ordering.
    expectDistance("vertical", {3,2}, {{0,0},{0,5}}, 3.0);
    expectDistance("diagonal", {1,0}, {{0,0},{2,2}}, std::sqrt(0.5));
    expectDistance("reversed", {2,3}, {{4,0},{0,0}}, 3.0);

    // Translation and sign of coordinates should not matter.
    expectDistance("translated", {12,7}, {{10,4},{14,4}}, 3.0);
    expectDistance("negative coordinates", {-2,-3}, {{-4,0},{0,0}}, 3.0);

    // Crossing and contact have a minimum distance of zero.
    expectSegmentDistance("perpendicular crossing",
                          {{-2,0},{2,0}},{{0,-3},{0,3}},0);
    expectSegmentDistance("endpoint contact",
                          {{0,0},{4,0}},{{4,0},{4,3}},0);
    expectSegmentDistance("collinear overlap",
                          {{0,0},{4,0}},{{2,0},{6,0}},0);

    // Separated parallel segments have the distance of their supports.
    expectSegmentDistance("parallel separation",
                          {{0,0},{4,0}},{{0,3},{4,3}},3);
    expectSegmentDistance("collinear separation",
                          {{0,0},{4,0}},{{7,0},{9,0}},3);

    // A closest endpoint can project to the interior of the other edge.
    expectSegmentDistance("endpoint to segment interior",
                          {{0,0},{4,0}},{{2,3},{2,5}},3);

    // The closest pair can be two endpoints of oblique segments.
    expectSegmentDistance("both endpoints",
                          {{0,0},{2,0}},{{5,4},{6,5}},5);

    // A zero-length segment is treated as a point.
    expectSegmentDistance("first degenerate",
                          {{2,3},{2,3}},{{0,0},{4,0}},3);
    expectSegmentDistance("both degenerate",
                          {{0,0},{0,0}},{{3,4},{3,4}},5);

    // Properly separated perpendicular lines can have nearby endpoints.
    expectSegmentDistance("non-intersecting perpendicular",
                          {{0,0},{4,0}},{{5,2},{5,4}},std::sqrt(5));

    // Phase 3.1C: distance to the *filled* polygon, not only its edges.
    const Polygon square{{{0,0},{4,0},{4,4},{0,4}}};
    const Polygon clockwiseSquare{{{0,4},{4,4},{4,0},{0,0}}};

    expectPolygonDistance("parallel above square",
                          {{-1,5},{5,5}},square,1);
    expectPolygonDistance("near convex corner",
                          {{5,5},{6,6}},square,std::sqrt(2.0));
    expectPolygonDistance("side near square",
                          {{5,1},{7,1}},square,1);

    // These must return zero even though the endpoints can both be outside.
    expectPolygonDistance("crosses interior",
                          {{-2,2},{6,2}},square,0);
    expectPolygonDistance("tangential vertex contact",
                          {{3,5},{5,3}},square,0);
    expectPolygonDistance("touches boundary edge",
                          {{-1,0},{1,0}},square,0);

    // Endpoint containment catches segments strictly inside the filled region.
    expectPolygonDistance("fully inside",
                          {{1,1},{3,3}},square,0);
    expectPolygonDistance("one endpoint inside",
                          {{2,2},{6,2}},square,0);
    expectPolygonDistance("zero length inside",
                          {{2,2},{2,2}},square,0);
    expectPolygonDistance("zero length outside",
                          {{2,7},{2,7}},square,3);

    expectPolygonDistance("clockwise winding",
                          {{-1,5},{5,5}},clockwiseSquare,1);

    // An L-shaped obstacle: the missing top-right quadrant is free space.
    const Polygon concave{{
        {0,0},{6,0},{6,2},{2,2},{2,6},{0,6}
    }};
    expectPolygonDistance("outside concave notch",
                          {{2.5,2.5},{5.5,2.5}},concave,0.5);
    expectPolygonDistance("inside concave arm",
                          {{0.5,1.5},{1.5,4.5}},concave,0);
    expectPolygonDistance("crosses concave interior",
                          {{1,5},{5,1}},concave,0);
    const Polygon reversedConcave{{
        {0,6},{2,6},{2,2},{6,2},{6,0},{0,0}
    }};
    expectPolygonDistance("concave clockwise winding",
                          {{2.5,2.5},{5.5,2.5}},reversedConcave,0.5);

    // Existing PATHLAB geometry ignores polygons with fewer than 3 vertices.
    const Polygon invalid{{{0,0},{2,0}}};
    if(!std::isinf(segmentPolygonDistance({{1,1},{3,1}}, invalid))){
        throw std::runtime_error("Invalid polygon must return +infinity");
    }

    // Phase 3.1D: strict continuous clearance (boundary contact is blocked).
    const std::vector<Polygon> oneSquare{square};
    expectClearance("radius zero, detached", {{-1,5},{5,5}}, oneSquare, 0.0, true);
    expectClearance("radius below distance", {{-1,5},{5,5}}, oneSquare, 0.99, true);
    expectClearance("radius exactly distance", {{-1,5},{5,5}}, oneSquare, 1.0, false);
    expectClearance("radius larger than distance", {{-1,5},{5,5}}, oneSquare, 1.01, false);
    expectClearance("two-unit separation", {{-1,6},{5,6}}, oneSquare, 1.0, true);
    expectClearance("radius equals two", {{-1,6},{5,6}}, oneSquare, 2.0, false);

    // Collision must reject even when r == 0, unlike original legacy visibility
    // graph paths that intentionally allow touching polygon vertices.
    expectClearance("interior traversal", {{-1,2},{5,2}}, oneSquare, 0.0, false);
    expectClearance("boundary contact", {{-1,4},{1,4}}, oneSquare, 0.0, false);
    expectClearance("fully inside", {{1,1},{3,3}}, oneSquare, 0.0, false);
    expectClearance("point touching corner", {{4,4},{4,4}}, oneSquare, 0.0, false);
    expectClearance("point strictly outside", {{5,2},{5,2}}, oneSquare, 0.5, true);
    expectClearance("point at clearance boundary", {{5,2},{5,2}}, oneSquare, 1.0, false);

    // Closest approach at a corner, rather than a projection onto an edge.
    expectClearance("corner margin passes", {{5,5},{6,6}}, oneSquare, 1.4, true);
    expectClearance("corner margin fails", {{5,5},{6,6}}, oneSquare, 1.5, false);

    // All obstacles must be checked; the nearest could be any polygon.
    const Polygon farSquare{{{10,0},{14,0},{14,4},{10,4}}};
    const std::vector<Polygon> twoSquares{square,farSquare};
    expectClearance("two obstacles, safe", {{9,5},{15,5}}, twoSquares, 0.5, true);
    expectClearance("second obstacle rejects", {{9,5},{15,5}}, twoSquares, 1.0, false);
    expectClearance("second obstacle crossed", {{9,2},{15,2}}, twoSquares, 0.0, false);

    // The concave notch is free, but it has its own nearest boundary.
    expectClearance("concave notch safe",
                    {{2.5,2.5},{5.5,2.5}}, {concave}, 0.49, true);
    expectClearance("concave notch boundary",
                    {{2.5,2.5},{5.5,2.5}}, {concave}, 0.5, false);
    expectClearance("clockwise shape",
                    {{-1,5},{5,5}}, {clockwiseSquare}, 1.0, false);

    // Empty scenes are safe, and invalid polygons follow the existing
    // convention that polygons with < 3 vertices do not form obstacles.
    expectClearance("no obstacles", {{0,0},{5,0}}, {}, 100.0, true);
    expectClearance("only degenerate polygon", {{1,1},{3,1}}, {invalid}, 100.0, true);
    expectClearance("invalid plus valid", {{-1,5},{5,5}},
                    {invalid,square}, 1.0, false);

    // Bad inputs must be rejected even when no obstacles exist.
    expectInvalidRadius("negative radius", -1.0);
    expectInvalidRadius("negative infinity",
                        -std::numeric_limits<double>::infinity());
    expectInvalidRadius("positive infinity",
                        std::numeric_limits<double>::infinity());
    expectInvalidRadius("NaN radius",
                        std::numeric_limits<double>::quiet_NaN());

    std::cout << "All geometric clearance distance and predicate tests passed.\n";
}
