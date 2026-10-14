#include "ClearanceVisibilityGraph.hpp"
#include "VisibilityGraph.hpp"
#include "../geometry/Clearance.hpp"
#include "../geometry/OffsetWaypoints.hpp"
#include <cmath>
#include <stdexcept>

Graph buildClearanceVisibilityGraph(
    const Point& start, const Point& goal,
    const std::vector<Polygon>& obstacles, double radius
){
    if(!std::isfinite(radius) || radius < 0.0)
        throw std::invalid_argument("Clearance radius must be finite and nonnegative");

    // Keep existing r=0 planner's boundary-contact semantics unchanged.
    if(radius == 0.0)
        return buildVisibilityGraph(start,goal,obstacles);

    Graph graph;
    graph.nodes.push_back({start});
    graph.nodes.push_back({goal});

    // Invalid endpoints cannot participate in a safe route.
    if(!segmentHasClearance({start,start},obstacles,radius) ||
       !segmentHasClearance({goal,goal},obstacles,radius))
        return graph;

    for(const Point& candidate:generateClearanceWaypoints(obstacles,radius)){
        bool duplicate=false;
        for(const GraphNode& node:graph.nodes){
            if(std::hypot(candidate.x-node.position.x,
                          candidate.y-node.position.y)<1e-6){
                duplicate=true;
                break;
            }
        }
        if(!duplicate)graph.nodes.push_back({candidate});
    }
    // Build a complete straight-line visibility graph over safe samples.
    // This implicitly handles overlapping expanded obstacle regions:
    // every candidate and edge must be safe from EVERY original polygon.
    for(std::size_t i=0;i<graph.nodes.size();++i){
        for(std::size_t j=i+1;j<graph.nodes.size();++j){
            const Point& a=graph.nodes[i].position;
            const Point& b=graph.nodes[j].position;
            if(segmentHasClearance({a,b},obstacles,radius)){
                const double weight=std::hypot(a.x-b.x,a.y-b.y);
                graph.edges.push_back({i,j,weight});
            }
        }
    }
    return graph;
}
