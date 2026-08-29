#include <vector>

#include "Graph.hpp"
#include "../geometry/Point.hpp"
#include "../geometry/Polygon.hpp"

std::vector<GraphNode> buildVisibilityNodes(
    const Point& start,
    const Point& goal,
    const std::vector<Polygon>& obstacles
);

std::vector<GraphEdge> buildVisibilityEdges(
    const std::vector<GraphNode>& nodes,
    const std::vector<Polygon>& obstacles
);

Graph buildVisibilityGraph(
    const Point& start,
    const Point& goal,
    const std::vector<Polygon>& obstacles
);