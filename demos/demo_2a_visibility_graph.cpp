#include <SFML/Graphics.hpp>

#include "../src/geometry/Point.hpp"
#include "../src/geometry/Polygon.hpp"
#include "../src/graph/VisibilityGraph.hpp"
#include "../src/visualization/Renderer.hpp"

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "Demo 2A - Visibility Graph"
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
