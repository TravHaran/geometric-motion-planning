#include "geometry/OffsetWaypoints.hpp"
#include "geometry/Clearance.hpp"

#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void require(bool condition, const std::string& description){
    if(!condition)throw std::runtime_error(description);
}
bool approximately(const Point& a,const Point& b,double tolerance=0.001){
    return std::hypot(a.x-b.x,a.y-b.y)<tolerance;
}
bool containsNear(const std::vector<Point>& points,const Point& target,
                  double tolerance=0.001){
    for(const Point& p:points)
        if(approximately(p,target,tolerance))return true;
    return false;
}
void checkEveryCandidate(const std::vector<Point>& points,
                         const std::vector<Polygon>& obstacles,double radius){
    for(const Point& p:points){
        require(std::isfinite(p.x) && std::isfinite(p.y),
                "generated waypoint must be finite");
        require(segmentHasClearance({p,p},obstacles,radius),
                "waypoint must be strictly outside EVERY inflated obstacle");
    }
}
void expectBadRadius(double radius){
    bool threw=false;
    try{
        (void)generateClearanceWaypoints({},radius);
    }catch(const std::invalid_argument&){
        threw=true;
    }
    require(threw,"invalid radius must throw even for empty obstacle scenes");
}
}
int main(){
    constexpr double radius=1.0;
    const Polygon square{{{0,0},{4,0},{4,4},{0,4}}};
    const std::vector<Polygon> obstacles{square};
    const auto squarePoints=generateClearanceWaypoints(obstacles,radius);
    require(squarePoints.size()>=24,
            "a square should generate convex-corner arc samples");
    checkEveryCandidate(squarePoints,obstacles,radius);

    // Arc endpoints at the four corners are outside their circles.
    require(containsNear(squarePoints,{-1.0088,0},0.002),
            "left offset endpoint missing from bottom-left corner");
    require(containsNear(squarePoints,{0,-1.0088},0.002),
            "bottom offset endpoint missing from bottom-left corner");
    require(containsNear(squarePoints,{5.0088,4},0.002),
            "right offset endpoint missing from top-right corner");
    require(containsNear(squarePoints,{4,5.0088},0.002),
            "top offset endpoint missing from top-right corner");

    // A different winding direction gives equivalent waypoint locations.
    const Polygon clockwise{{{0,4},{4,4},{4,0},{0,0}}};
    const auto reversePoints=generateClearanceWaypoints({clockwise},radius);
    require(reversePoints.size()==squarePoints.size(),
            "winding direction changed number of offset candidates");
    for(const Point& p:squarePoints){
        require(containsNear(reversePoints,p,0.00001),
                "clockwise and counterclockwise waypoint sets differ");
    }
    checkEveryCandidate(reversePoints,{clockwise},radius);

    // L-shaped concave polygon: (2,2) is reflex. Its adjacent outward
    // offset supporting lines meet at (2+r+epsilon, 2+r+epsilon).
    const Polygon concave{{
        {0,0},{6,0},{6,2},{2,2},{2,6},{0,6}
    }};
    const auto concavePoints=generateClearanceWaypoints({concave},0.5);
    checkEveryCandidate(concavePoints,{concave},0.5);
    require(containsNear(concavePoints,{2.5001,2.5001},0.002),
            "concave reflex corner miter candidate missing");

    const Polygon clockwiseConcave{{
        {0,6},{2,6},{2,2},{6,2},{6,0},{0,0}
    }};
    const auto clockwiseConcavePoints=
        generateClearanceWaypoints({clockwiseConcave},0.5);
    require(containsNear(clockwiseConcavePoints,{2.5001,2.5001},0.002),
            "reflex waypoint must be independent of polygon winding");
    checkEveryCandidate(clockwiseConcavePoints,{clockwiseConcave},0.5);

    // Candidate filtering must consider other obstacles, not just the one
    // whose corner generated the point.
    const Polygon nearby{{{5,0},{9,0},{9,4},{5,4}}};
    const std::vector<Polygon> neighboring{square,nearby};
    const auto neighboringPoints=generateClearanceWaypoints(neighboring,1.0);
    checkEveryCandidate(neighboringPoints,neighboring,1.0);
    require(neighboringPoints.size() < squarePoints.size()*2,
            "overlapping clearance zones should eliminate some candidates");

    // The generator is deliberately disabled at r=0, where the original
    // graph still uses the actual obstacle vertices.
    require(generateClearanceWaypoints(obstacles,0.0).empty(),
            "zero-radius waypoints must remain the legacy graph's concern");
    require(generateClearanceWaypoints({},2.0).empty(),
            "no obstacles means no intermediate waypoints");
    const Polygon invalid{{{0,0},{1,0}}};
    require(generateClearanceWaypoints({invalid},1.0).empty(),
            "invalid polygons are ignored");
    expectBadRadius(-1.0);
    expectBadRadius(std::numeric_limits<double>::infinity());
    expectBadRadius(std::numeric_limits<double>::quiet_NaN());

    std::cout<<"All offset waypoint candidate tests passed.\n";
}
