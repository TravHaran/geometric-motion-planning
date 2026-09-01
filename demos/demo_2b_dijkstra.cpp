#include <SFML/Graphics.hpp>

#include "../src/geometry/Point.hpp"
#include "../src/geometry/Polygon.hpp"
#include "../src/graph/VisibilityGraph.hpp"
#include "../src/visualization/Renderer.hpp"
#include "../src/planners/Dijkstra.hpp"

/*
 * Demo 2B: Dijkstra Shortest Path
 *
 * This demo builds a visibility graph around a rectangular obstacle
 * and then runs Dijkstra's algorithm from the start node to the goal node.
 *
 * Expected visualization:
 *
 * - The start point appears on the left in green.
 * - The goal point appears on the right in red.
 * - A rectangular obstacle blocks the direct path between them.
 * - All valid visibility-graph edges are shown in gray.
 * - Obstacle boundaries are shown in red.
 * - Visibility-graph nodes are shown in white.
 * - The shortest path found by Dijkstra is highlighted in green
 *   and should travel around either the top or bottom of the obstacle.
 *
 * Since the environment is symmetric, the top and bottom routes have
 * equal length. Which one Dijkstra returns depends on graph/edge ordering.
 */

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "Demo 2B - Dijkstra Shortest Path"
    );

    Point start{100.0, 300.0};
    Point goal{700.0, 300.0};

    Polygon obstacle{{
        {300.0, 200.0},
        {500.0, 200.0},
        {500.0, 400.0},
        {300.0, 400.0}
    }};

    std::vector<Polygon> obstacles{
        obstacle
    };

    Graph graph =
        buildVisibilityGraph(
            start,
            goal,
            obstacles
        );
    
    DijkstraResult result =
        dijkstra(
            graph,
            0,
            1
        );

    while(window.isOpen()){

        while(const std::optional event = window.pollEvent()){
            if(event->is<sf::Event::Closed>()){
                window.close();
            }
        }

        window.clear();

        // Draw visibility edges.
        drawGraph(window, graph, sf::Color(120, 120, 120));

        // Draw obstacle edges.
        for(const Polygon& currentObstacle : obstacles){

            const std::size_t n = currentObstacle.vertices.size();

            for(std::size_t i = 0; i < n; ++i){

                Segment edge{
                    currentObstacle.vertices[i],
                    currentObstacle.vertices[(i + 1) % n]
                };

                drawSegment(
                    window,
                    edge,
                    sf::Color::Red
                );
            }
        }

        // Draw Dijkstra's shortest path
        for(std::size_t i = 0; i + 1 < result.path.size(); ++i){
            std::size_t fromNode =
                result.path[i];

            std::size_t toNode =
                result.path[i + 1];

            const Point& a =
                graph.nodes[fromNode].position;

            const Point& b =
                graph.nodes[toNode].position;

            Segment pathSegment{a, b};

            drawSegment(
                window,
                pathSegment,
                sf::Color::Green
            );
        }

        // Draw graph nodes.
        for(const GraphNode& node : graph.nodes){
            drawPoint(
                window,
                node.position,
                sf::Color::White
            );
        }

        // Draw start and goal on top so they stand out.
        drawPoint(
            window,
            start,
            sf::Color::Green
        );

        drawPoint(
            window,
            goal,
            sf::Color::Red
        );

        window.display();
    }

    return 0;
}
