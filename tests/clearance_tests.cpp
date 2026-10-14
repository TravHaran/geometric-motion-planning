#include "geometry/Clearance.hpp"

#include <cmath>
#include <iostream>
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

    std::cout << "All point/segment clearance primitive tests passed.\n";
}
