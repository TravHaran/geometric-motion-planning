#include "VisibilityGraph.hpp"
#include "../geometry/Clearance.hpp"
#include "../geometry/Geometry.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {
// A *conservative* polygonal approximation of the disk-offset boundary:
// for adjacent sampled directions separated by delta, choose a radius
// (r + slack) / cos(delta/2), so their connecting chord stays outside
// the true radius-r disk. This is a graph approximation; it does NOT
// claim to be the exact shortest-path graph for a curved offset boundary.
constexpr double PI=3.1415926535897932384626433832795;
constexpr double MAX_ARC_STEP=PI/12.0; // 15 degrees
double distance(const Point& a,const Point& b){
    return std::hypot(b.x-a.x,b.y-a.y);
}
double signedArea2(const Polygon& poly){
    double sum=0;
    for(std::size_t i=0;i<poly.vertices.size();++i){
        const auto& a=poly.vertices[i];
        const auto& b=poly.vertices[(i+1)%poly.vertices.size()];
        sum+=a.x*b.y-b.x*a.y;
    }
    return sum;
}
Point outwardNormal(Point a,Point b,double orientationSign){
    const double dx=b.x-a.x,dy=b.y-a.y;
    const double len=std::hypot(dx,dy);
    if(len<1e-12)return {0,0};
    // CCW polygon has its interior to the left of each boundary edge.
    return orientationSign>0?Point{dy/len,-dx/len}:Point{-dy/len,dx/len};
}
std::vector<GraphNode> offsetCandidates(const std::vector<Polygon>& obstacles,
                                         double clearance){
    std::vector<GraphNode> nodes;
    // A small clearance slack prevents floating-point boundary rejection,
    // and keeps candidate vertices strictly outside the forbidden region.
    const double slack=std::max(0.0001,clearance*0.00001);
    for(const Polygon& poly:obstacles){
        if(poly.vertices.size()<3)continue;
        const double area=signedArea2(poly);
        if(std::abs(area)<1e-12)continue;
        const double sign=area>0?1.0:-1.0;
        for(std::size_t i=0;i<poly.vertices.size();++i){
            const Point& prev=poly.vertices[(i+poly.vertices.size()-1)%poly.vertices.size()];
            const Point& v=poly.vertices[i];
            const Point& next=poly.vertices[(i+1)%poly.vertices.size()];
            const double len1=distance(prev,v),len2=distance(v,next);
            if(len1<1e-12||len2<1e-12)continue;
            const double turn=orientation(prev,v,next)*sign;
            // Reflex vertices do not generate an exterior circular arc
            // on the boundary of the convex-disk offset of this polygon.
            if(turn<=len1*len2*1e-10)continue;
            Point n1=outwardNormal(prev,v,sign);
            Point n2=outwardNormal(v,next,sign);
            double a1=std::atan2(n1.y,n1.x);
            double a2=std::atan2(n2.y,n2.x);
            if(sign>0){while(a2<a1)a2+=2.0*PI;}
            else{while(a2>a1)a2-=2.0*PI;}
            const double sweep=std::abs(a2-a1);
            const int steps=std::max(1,static_cast<int>(std::ceil(sweep/MAX_ARC_STEP)));
            const double dt=(a2-a1)/static_cast<double>(steps);
            const double rho=(clearance+slack)/std::cos(std::abs(dt)*0.5);
            for(int j=0;j<=steps;++j){
                const double angle=a1+dt*static_cast<double>(j);
                nodes.push_back({{v.x+rho*std::cos(angle),
                                  v.y+rho*std::sin(angle)}});
            }
        }
    }
    return nodes;
}
}

Graph buildClearanceVisibilityGraph(
    const Point& start,
    const Point& goal,
    const std::vector<Polygon>& obstacles,
    double clearance
){
    if(!std::isfinite(clearance)||clearance<0)
        throw std::invalid_argument("Clearance must be nonnegative and finite");
    // Preserve original visibility graph at r = 0, including vertex contacts.
    if(clearance==0.0)return buildVisibilityGraph(start,goal,obstacles);

    Graph graph;
    graph.nodes.push_back({start});
    graph.nodes.push_back({goal});
    if(!pointHasClearance(start,obstacles,clearance) ||
       !pointHasClearance(goal,obstacles,clearance))
        return graph; // invalid endpoints -> no possible edges

    for(const GraphNode& node:offsetCandidates(obstacles,clearance)){
        if(!pointHasClearance(node.position,obstacles,clearance))continue;
        bool unique=true;
        for(const auto& existing:graph.nodes){
            if(distance(node.position,existing.position)<1e-6){
                unique=false;break;
            }
        }
        if(unique)graph.nodes.push_back(node);
    }

    // Complete visibility graph over the safe candidate configurations.
    // Unlike the original graph, a positive-clearance edge may never enter
    // a disk-expanded obstacle, including between its two endpoints.
    for(std::size_t i=0;i<graph.nodes.size();++i){
        for(std::size_t j=i+1;j<graph.nodes.size();++j){
            const Point& a=graph.nodes[i].position;
            const Point& b=graph.nodes[j].position;
            if(segmentHasClearance({a,b},obstacles,clearance)){
                graph.edges.push_back({i,j,distance(a,b)});
            }
        }
    }
    return graph;
}
