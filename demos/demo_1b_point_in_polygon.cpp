#include <SFML/Graphics.hpp>
#include <iostream>
#include <optional>

#include "geometry/Polygon.hpp"
#include "geometry/Segment.hpp"
#include "geometry/Geometry.hpp"
#include "visualization/Renderer.hpp"

int main(){
    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "Demo 1B - Point in Polygon"
    );

    Polygon polygon;
    bool polygonFinalized = false;
    Point queryPoint;
    bool queryPointSet = false;

    while (window.isOpen()){
        // -------------------------
        // Handle input/events
        // -------------------------
        while (const std::optional event = window.pollEvent()){
            // Close the window
            if (event->is<sf::Event::Closed>()){
                window.close();
            }
            // Mouse click
            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()){
                // only want left-clicks, and only while the polygon is still being constructed
                if (
                    mousePressed->button == sf::Mouse::Button::Left &&
                    !polygonFinalized
                ){
                    Point clickedPoint{
                        static_cast<double>(mousePressed->position.x),
                        static_cast<double>(mousePressed->position.y)
                    };
                    polygon.vertices.push_back(clickedPoint);

                    std::cout
                        << "Added vertex: ("
                        << clickedPoint.x << ", "
                        << clickedPoint.y << ")\n";
                }
                // right-click
                if (
                    mousePressed->button == sf::Mouse::Button::Right &&
                    polygonFinalized
                ){
                    queryPoint = Point{
                        static_cast<double>(mousePressed->position.x),
                        static_cast<double>(mousePressed->position.y)
                    };

                    queryPointSet = true;
                }
                bool inside = pointInPolygon(queryPoint, polygon);
                std::cout
                    << "Query point: ("
                    << queryPoint.x << ", "
                    << queryPoint.y << ") -> "
                    << (inside ? "inside" : "outside")
                    << "\n";
            }
            // Keyboard input
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()){
                // Enter should finalize polygon
                if (
                    keyPressed->code == sf::Keyboard::Key::Enter &&
                    polygon.vertices.size() >= 3 &&// can't make a polygon from one or two points
                    !polygonFinalized
                ){
                    polygonFinalized = true;
                    std::cout << "Polygon finalized.\n";
                }
                // R should reset
                if (keyPressed->code == sf::Keyboard::Key::R)
                {
                    polygon.vertices.clear();
                    polygonFinalized = false;
                    queryPointSet = false;

                    std::cout << "Polygon cleared.\n";
                }
            }
        }
        // -------------------------
        // Draw everything
        // -------------------------
        window.clear();
        // draw every polygon vertex
        for (const Point& point : polygon.vertices){
            drawPoint(window, point);
        }
        // draw edges
        for (std::size_t i = 0; i + 1 < polygon.vertices.size(); i++){
            Segment edge{
                polygon.vertices[i],
                polygon.vertices[i + 1]
            };
            drawSegment(window, edge);
        }
        if (polygonFinalized){
            Segment closingEdge{
                polygon.vertices.back(),
                polygon.vertices.front()
            };

            drawSegment(window, closingEdge);
        }
        if (queryPointSet){
            drawPoint(window, queryPoint);
        }

        window.display();
    }

    return 0;
}