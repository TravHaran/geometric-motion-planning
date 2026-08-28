#include "Geometry.hpp"

#include <algorithm>
#include <cmath>

/*
double arithmetic can produce something like:
0.000000000000000027
when mathematically the answer should be zero
this is relevant if we're getting coordinates from a visualizer 
and doing more complicated calculations

so we'll introduce and epsilon and ask |value| < ϵ
*/
constexpr double EPSILON = 1e-9;

bool approximatelyZero(double val){
    return std::abs(val) < EPSILON;
}

/*
Current geometric predicates use an epsilon for collinearity tests 
but are not designed as exact/robust predicates for numerically 
pathological inputs.
*/

double orientation(
    const Point& a,
    const Point& b, 
    const Point& c
){
    //orientation(A, B, C) = (x2-x1)(y3-y1) - (y2-y1)(x3-x1)
    return (b.x-a.x)*(c.y-a.y) - (b.y-a.y)*(c.x-a.x);
}

bool onSegment(
    const Point& a,
    const Point& b, 
    const Point& p
){
    return approximatelyZero(orientation(a, b, p)) &&
            std::min(a.x, b.x) <= p.x &&
            p.x <= std::max(a.x, b.x) &&
            std::min(a.y, b.y) <= p.y &&
            p.y <= std::max(a.y, b.y);
}

bool segmentsIntersect(
    const Segment& s1,
    const Segment& s2
){
    const Point& A = s1.a;
    const Point& B = s1.b;
    const Point& C = s2.a;
    const Point& D = s2.b;

    double o1 = orientation(A, B, C);
    double o2 = orientation(A, B, D);
    double o3 = orientation(C, D, A);
    double o4 = orientation(C, D, B);
    
    // 1. General crossing case
    // o1 and o2 have opposite signs
    // o3 and o4 have opposite signs
    if(
        ((o1 < 0 && o2 > 0) || (o1 > 0 && o2 < 0) ) &&
        ((o3 < 0 && o4 > 0) || (o3 > 0 && o4 < 0) )
    ) return true;

    // Collinear/touching cases
    if(onSegment(A, B, C)) return true;
    if(onSegment(A, B, D)) return true;
    if(onSegment(C, D, A)) return true;
    if(onSegment(C, D, B)) return true;

    return false;
}

std::vector<Segment> polygonEdges(const Polygon& polygon) {
    std::vector<Segment> edges;
    std::size_t len = polygon.vertices.size();
    if(len < 3) return {};
    for(std::size_t i = 0; i < len; i++){
        Segment seg = {polygon.vertices[i], polygon.vertices[(i+1) %len]};
        edges.push_back(seg);
    }
    return edges;
}

bool segmentIntersectsPolygon(const Segment& segment, const Polygon& polygon){
    // Get polygon edges
    std::vector<Segment> edges = polygonEdges(polygon);
    // check each edge for for intersection
    for(const Segment& edge : edges){
        if(segmentsIntersect(segment, edge)) return true;
    }
    return false;
}

bool pointInPolygon(
    const Point& point,
    const Polygon& polygon
){
    // invalid polygon check
    if (polygon.vertices.size() < 3) {
        return false;
    }

    // Use Winding Number algorithm to determine if a point is in a polygon
    int windingNumber = 0;
    std::vector<Segment> edges = polygonEdges(polygon);

    for (const Segment& edge: edges){
        const Point& A = edge.a;
        const Point& B = edge.b;

        // edge case
        // if point lies directly on an edge, count it as inside
        if(onSegment(A, B, point)){
            return true;
        }

        // upward crossing
        if(A.y <= point.y && B.y > point.y){
            if(orientation(A, B, point) > 0) {
                // point is to the left of edge AB
                windingNumber++;
            }
        }
        // downward crossing
        else if(B.y <= point.y && A.y > point.y){
            if(orientation(A, B, point) < 0) {
                // point is to the right of edge AB
                windingNumber--;
            }
        }
    }
    return windingNumber != 0;
}

bool isSegmentCollisionFree(
    const Segment& segment,
    const std::vector<Polygon>& obstacles
){
    for(const Polygon& obstacle : obstacles){
        if(
            segmentIntersectsPolygon(segment, obstacle) ||
            pointInPolygon(segment.a, obstacle) ||
            pointInPolygon(segment.b, obstacle)
        ) 
        return false;
    }
    return true;
}

bool isPathCollisionFree(
    const Path& path,
    const std::vector<Polygon>& obstacles
){  
    if (path.waypoints.empty()) return true; // empty path

    if (path.waypoints.size() == 1){ // single point path
        for (const Polygon& obstacle : obstacles){
            if (pointInPolygon(path.waypoints[0], obstacle)){
                return false;
            }
        }

        return true;
    }

    // check every consecutive pair for collisions

    for(std::size_t i = 0; i + 1 < path.waypoints.size(); i++){
        Segment segment{
            path.waypoints[i],
            path.waypoints[i + 1]
        };

        if(!isSegmentCollisionFree(segment, obstacles)){
            return false;
        }
    }
    return true;
}

