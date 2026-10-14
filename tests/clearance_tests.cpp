#include "../src/geometry/Clearance.hpp"
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <vector>

int main() {
    const Polygon square{{{0,0},{2,0},{2,2},{0,2}}};
    const std::vector<Polygon> obstacles{square};
    const auto near = [](double a, double b) {return std::abs(a-b)<1e-10;};
    assert(near(pointSegmentDistance({1,3},{{0,0},{2,0}}),3.0));
    assert(near(pointSegmentDistance({3,4},{{0,0},{0,0}}),5.0));
    assert(near(pointPolygonDistance({1,1},square),0.0));
    assert(near(pointPolygonDistance({3,1},square),1.0));
    assert(near(pointPolygonDistance({3,3},square),std::sqrt(2.0)));
    assert(near(segmentPolygonDistance({{-1,1},{3,1}},square),0.0));
    assert(near(segmentPolygonDistance({{-1,3},{3,3}},square),1.0));
    assert(near(segmentPolygonDistance({{1,1},{1,1}},square),0.0));
    assert(!isPointClearanceFree({3,1},obstacles,1.0));
    assert(isPointClearanceFree({3,1},obstacles,0.99));
    assert(!isSegmentClearanceFree({{-1,3},{3,3}},obstacles,1.0));
    assert(isSegmentClearanceFree({{-1,3},{3,3}},obstacles,0.99));
    assert(!isSegmentClearanceFree({{-1,1},{3,1}},obstacles,0.0));
    assert(isSegmentClearanceFree({{-1,3},{3,3}},obstacles,0.0));
    bool threw = false;
    try { (void)isPointClearanceFree({0,0}, obstacles,-1.0); }
    catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
}
