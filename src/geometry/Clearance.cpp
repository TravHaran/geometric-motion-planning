#include "Clearance.hpp"
#include "Geometry.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace {
double distance(const Point& a,const Point& b){
    return std::hypot(a.x-b.x,a.y-b.y);
}
void validateRadius(double r){
    if(!std::isfinite(r)||r<0.0)
        throw std::invalid_argument("Clearance radius must be finite and >= 0");
}
}
double pointSegmentDistance(const Point& p,const Segment& edge){
    const double dx=edge.b.x-edge.a.x,dy=edge.b.y-edge.a.y;
    const double len2=dx*dx+dy*dy;
    if(len2==0.0)return distance(p,edge.a);
    const double dot=(p.x-edge.a.x)*dx+(p.y-edge.a.y)*dy;
    const double t=std::clamp(dot/len2,0.0,1.0);
    return distance(p,{edge.a.x+t*dx,edge.a.y+t*dy});
}
double segmentSegmentDistance(const Segment& a,const Segment& b){
    if(segmentsIntersect(a,b))return 0.0;
    return std::min({pointSegmentDistance(a.a,b),pointSegmentDistance(a.b,b),
                     pointSegmentDistance(b.a,a),pointSegmentDistance(b.b,a)});
}
double pointPolygonDistance(const Point& p,const Polygon& polygon){
    if(polygon.vertices.size()<3)return std::numeric_limits<double>::infinity();
    if(pointInPolygon(p,polygon))return 0.0;
    double best=std::numeric_limits<double>::infinity();
    for(std::size_t i=0;i<polygon.vertices.size();++i){
        const Segment edge{polygon.vertices[i],
                            polygon.vertices[(i+1)%polygon.vertices.size()]};
        best=std::min(best,pointSegmentDistance(p,edge));
    }
    return best;
}
double segmentPolygonDistance(const Segment& segment,const Polygon& polygon){
    if(polygon.vertices.size()<3)return std::numeric_limits<double>::infinity();
    if(pointInPolygon(segment.a,polygon)||pointInPolygon(segment.b,polygon))
        return 0.0;
    double best=std::numeric_limits<double>::infinity();
    for(std::size_t i=0;i<polygon.vertices.size();++i){
        const Segment edge{polygon.vertices[i],
                            polygon.vertices[(i+1)%polygon.vertices.size()]};
        if(segmentsIntersect(segment,edge))return 0.0;
        best=std::min(best,segmentSegmentDistance(segment,edge));
    }
    return best;
}
bool pointHasClearance(const Point& p,const std::vector<Polygon>& obstacles,double radius){
    validateRadius(radius);
    for(const Polygon& poly:obstacles){
        if(pointPolygonDistance(p,poly)<=radius)return false;
    }
    return true;
}
bool segmentHasClearance(const Segment& edge,const std::vector<Polygon>& obstacles,double radius){
    validateRadius(radius);
    for(const Polygon& poly:obstacles){
        if(segmentPolygonDistance(edge,poly)<=radius)return false;
    }
    return true;
}
