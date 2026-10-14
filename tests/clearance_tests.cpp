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

    std::cout << "All point-to-segment clearance tests passed.\n";
}
