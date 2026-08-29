#include "Dijkstra.hpp"

#include <algorithm>
#include <limits>
#include <vector>

/*
 * Dijkstra's Algorithm
 *
 * Goal:
 * Find the shortest path from a start node to a goal node
 * in a graph with non-negative edge weights.
 *
 * Main idea:
 *
 * 1. Set the start node distance to 0.
 * 2. Set every other node distance to infinity.
 * 3. Repeatedly select the unvisited node with the
 *    smallest known distance.
 * 4. Relax all edges connected to that node.
 * 5. Store parent information whenever a shorter route
 *    to a neighboring node is found.
 * 6. Once the goal is finalized, reconstruct the path
 *    by following parent links backward.
 */


/*
 * Find the unvisited node with the smallest tentative distance.
 *
 * Returns nodeCount if no reachable unvisited node remains.
 */
std::size_t findClosestUnvisitedNode(
    const std::vector<double>& distances,
    const std::vector<bool>& visited
){
    const std::size_t nodeCount = distances.size();

    // nodeCount is used as an invalid node index.
    std::size_t closestNode = nodeCount;

    double closestDistance =
        std::numeric_limits<double>::infinity();

    for(std::size_t i = 0; i < nodeCount; ++i){

        // Already-finalized nodes should not be considered.
        if(visited[i]){
            continue;
        }

        // Keep the unvisited node with the smallest
        // currently known distance.
        if(distances[i] < closestDistance){
            closestDistance = distances[i];
            closestNode = i;
        }
    }

    return closestNode;
}


DijkstraResult dijkstra(
    const Graph& graph,
    std::size_t startNode,
    std::size_t goalNode
){
    const std::size_t nodeCount =
        graph.nodes.size();

    /*
     * distance[i]
     *
     * Shortest currently known distance from the
     * start node to node i.
     *
     * Initially every node is unreachable.
     */
    std::vector<double> distances(
        nodeCount,
        std::numeric_limits<double>::infinity()
    );

    /*
     * visited[i]
     *
     * true means the shortest distance to node i
     * has already been finalized.
     */
    std::vector<bool> visited(
        nodeCount,
        false
    );

    /*
     * parent[i]
     *
     * Stores the node used to reach i through the
     * currently best known path.
     *
     * nodeCount is used as an invalid parent index
     * because valid indices are:
     *
     *     0 ... nodeCount - 1
     */
    std::vector<std::size_t> parents(
        nodeCount,
        nodeCount
    );

    // The distance from the start node to itself is zero.
    distances[startNode] = 0.0;


    /*
     * Main Dijkstra loop
     */
    while(true){

        // Select the reachable unvisited node with the
        // smallest tentative distance.
        std::size_t currentNode =
            findClosestUnvisitedNode(
                distances,
                visited
            );

        // No reachable unvisited nodes remain.
        if(currentNode == nodeCount){
            break;
        }

        /*
         * If the closest node is the goal, its shortest
         * distance has now been finalized.
         *
         * We do not need to explore edges leaving the goal.
         */
        if(currentNode == goalNode){
            break;
        }

        // The shortest distance to this node is now final.
        visited[currentNode] = true;


        /*
         * Examine every edge connected to currentNode.
         *
         * Our GraphEdge representation stores each
         * undirected edge only once:
         *
         *     {from, to, weight}
         *
         * Therefore currentNode may appear as either
         * edge.from or edge.to.
         */
        for(const GraphEdge& edge : graph.edges){

            // Invalid value means this edge is not connected
            // to currentNode.
            std::size_t neighbor = nodeCount;

            if(edge.from == currentNode){
                neighbor = edge.to;
            }
            else if(edge.to == currentNode){
                neighbor = edge.from;
            }
            else{
                continue;
            }

            // No need to relax already-finalized nodes.
            if(visited[neighbor]){
                continue;
            }


            /*
             * Edge relaxation
             *
             * Ask:
             *
             * Is going from
             *
             * start -> ... -> currentNode -> neighbor
             *
             * cheaper than the best route to neighbor
             * that we already know?
             */
            double newDistance =
                distances[currentNode]
                + edge.weight;

            if(newDistance < distances[neighbor]){

                // A shorter route has been found.
                distances[neighbor] =
                    newDistance;

                // Remember how we reached this node.
                parents[neighbor] =
                    currentNode;
            }
        }
    }


    /*
     * Path reconstruction
     */
    std::vector<std::size_t> path;

    // If the goal distance is still infinity, there is
    // no path from startNode to goalNode.
    if(
        distances[goalNode] ==
        std::numeric_limits<double>::infinity()
    ){
        return {
            std::numeric_limits<double>::infinity(),
            path
        };
    }


    /*
     * Follow parent links backward:
     *
     * goal -> parent -> parent -> ... -> start
     */
    std::size_t currentNode = goalNode;

    while(currentNode != startNode){

        path.push_back(currentNode);

        currentNode =
            parents[currentNode];
    }

    // Add the start node itself.
    path.push_back(startNode);


    /*
     * The path is currently backward:
     *
     *     goal -> ... -> start
     *
     * Reverse it so the returned path becomes:
     *
     *     start -> ... -> goal
     */
    std::reverse(
        path.begin(),
        path.end()
    );


    return {
        distances[goalNode],
        path
    };
}