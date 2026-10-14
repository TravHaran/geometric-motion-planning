#include "graph/ClearanceVisibilityGraph.hpp"
#include "graph/VisibilityGraph.hpp"
#include "geometry/Clearance.hpp"
#include "planners/Dijkstra.hpp"
#include "planners/AStar.hpp"
#include "planners/BFS.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
void require(bool b,const char* message){
    if(!b)throw std::runtime_error(message);
}
void verifyGraph(const Graph& graph,const std::vector<Polygon>& obs,double radius){
    require(graph.nodes.size()>=2,"missing endpoints");
    for(const auto& edge:graph.edges){
        require(edge.from<graph.nodes.size() && edge.to<graph.nodes.size(),
                "invalid edge index");
        const auto a=graph.nodes[edge.from].position,b=graph.nodes[edge.to].position;
        require(segmentHasClearance({a,b},obs,radius),"unsafe clearance edge");
        require(std::abs(edge.weight-std::hypot(b.x-a.x,b.y-a.y))<1e-8,
                "incorrect Euclidean weight");
    }
}
bool hasDirect(const Graph& graph){
    for(const auto& e:graph.edges)
        if((e.from==0&&e.to==1)||(e.from==1&&e.to==0))return true;
    return false;
}
}
int main(){
    const Polygon square{{{2,2},{4,2},{4,4},{2,4}}};
    const std::vector<Polygon> obstacles{square};
    const Point a{0,3},b{6,3};

    // Straight start-goal collision requires a genuine detour.
    const Graph graph=buildClearanceVisibilityGraph(a,b,obstacles,0.5);
    require(graph.nodes.size()>2,"must generate offset waypoint nodes");
    verifyGraph(graph,obstacles,0.5);
    require(!hasDirect(graph),"direct crossing incorrectly admitted");
    const auto dj=dijkstra(graph,0,1);
    const auto ast=aStar(graph,0,1);
    const auto bf=bfs(graph,0,1);
    require(dj.path.size()>=3,"Dijkstra failed to find detour");
    require(ast.path.size()>=3,"A* failed to find detour");
    require(bf.path.size()>=3,"BFS failed to find detour");
    for(std::size_t i=1;i<dj.path.size();++i){
        require(segmentHasClearance(
            {graph.nodes[dj.path[i-1]].position,graph.nodes[dj.path[i]].position},
            obstacles,0.5),"Dijkstra path failed geometric clearance");
    }

    // The old visibility graph remains bit-for-bit semantically unaffected at r=0.
    const Graph legacy=buildVisibilityGraph(a,b,obstacles);
    const Graph zero=buildClearanceVisibilityGraph(a,b,obstacles,0);
    require(zero.nodes.size()==legacy.nodes.size(),"zero-radius nodes changed");
    require(zero.edges.size()==legacy.edges.size(),"zero-radius edges changed");

    // Unsafe start and goal must have no reachable graph edges.
    const Graph invalidStart=buildClearanceVisibilityGraph({2.2,3},b,obstacles,0.5);
    const Graph invalidGoal=buildClearanceVisibilityGraph(a,{4.2,3},obstacles,0.5);
    require(invalidStart.edges.empty(),"unsafe start should be disconnected");
    require(invalidGoal.edges.empty(),"unsafe goal should be disconnected");

    // Empty scenes have just one direct connection.
    const Graph empty=buildClearanceVisibilityGraph(a,b,{},50);
    require(empty.nodes.size()==2&&empty.edges.size()==1,
            "empty scene should connect start and goal");

    // Neighboring obstacles with overlapping disk-offset bands.
    const Polygon neighbor{{{4.7,2},{6.7,2},{6.7,4},{4.7,4}}};
    const std::vector<Polygon> both{square,neighbor};
    const Graph overlapping=buildClearanceVisibilityGraph({0,3},{9,3},both,0.5);
    verifyGraph(overlapping,both,0.5);
    require(!hasDirect(overlapping),"overlapping buffers leaked direct edge");

    // A narrow corridor at less than twice the radius must be blocked
    // by the geometric validator, independently of sampling resolution.
    const Polygon upper{{{0,2},{4,2},{4,3},{0,3}}};
    const Polygon lower{{{0,0},{4,0},{4,1},{0,1}}};
    const std::vector<Polygon> corridor{upper,lower};
    require(!segmentHasClearance({{0,1.5},{4,1.5}},corridor,0.51),
            "narrow corridor must be blocked");
    require(segmentHasClearance({{0,1.5},{4,1.5}},corridor,0.49),
            "wide-enough corridor incorrectly blocked");

    std::cout<<"All clearance-aware graph integration tests passed.\n";
}
