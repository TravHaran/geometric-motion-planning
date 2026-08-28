#include <SFML/Graphics.hpp>

#include <iostream>
#include <optional>

#include "geometry/Polygon.hpp"
#include "geometry/Path.hpp"
#include "geometry/Segment.hpp"
#include "geometry/Geometry.hpp"
#include "visualization/Renderer.hpp"

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "Demo 1C - Path Collision"
    );

    Polygon obstacle;
    bool obstacleFinalized = false;

    Path path;

    while (window.isOpen())
    {
        // -------------------------
        // Handle input/events
        // -------------------------
        while (const std::optional event = window.pollEvent())
        {
            // Close the window
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            // Mouse input
            if (const auto* mousePressed =
                    event->getIf<sf::Event::MouseButtonPressed>())
            {
                // Left-click:
                // build the obstacle before it is finalized
                if (
                    mousePressed->button == sf::Mouse::Button::Left &&
                    !obstacleFinalized
                )
                {
                    Point clickedPoint{
                        static_cast<double>(mousePressed->position.x),
                        static_cast<double>(mousePressed->position.y)
                    };

                    obstacle.vertices.push_back(clickedPoint);

                    std::cout
                        << "Added obstacle vertex: ("
                        << clickedPoint.x << ", "
                        << clickedPoint.y << ")\n";
                }

                // Right-click:
                // add path waypoints after the obstacle is finalized
                if (
                    mousePressed->button == sf::Mouse::Button::Right &&
                    obstacleFinalized
                )
                {
                    Point waypoint{
                        static_cast<double>(mousePressed->position.x),
                        static_cast<double>(mousePressed->position.y)
                    };

                    path.waypoints.push_back(waypoint);

                    std::cout
                        << "Added path waypoint: ("
                        << waypoint.x << ", "
                        << waypoint.y << ")\n";
                }
            }

            // Keyboard input
            if (const auto* keyPressed =
                    event->getIf<sf::Event::KeyPressed>())
            {
                // Enter finalizes the obstacle
                if (
                    keyPressed->code == sf::Keyboard::Key::Enter &&
                    obstacle.vertices.size() >= 3 &&
                    !obstacleFinalized
                )
                {
                    obstacleFinalized = true;

                    std::cout << "Obstacle finalized.\n";
                }

                // R resets everything
                if (keyPressed->code == sf::Keyboard::Key::R)
                {
                    obstacle.vertices.clear();
                    obstacleFinalized = false;

                    path.waypoints.clear();

                    std::cout << "Demo reset.\n";
                }
            }
        }

        // -------------------------
        // Draw everything
        // -------------------------
        window.clear();

        // Draw obstacle vertices
        for (const Point& point : obstacle.vertices)
        {
            drawPoint(window, point);
        }

        // Draw consecutive obstacle edges
        for (
            std::size_t i = 0;
            i + 1 < obstacle.vertices.size();
            i++
        )
        {
            Segment edge{
                obstacle.vertices[i],
                obstacle.vertices[i + 1]
            };

            drawSegment(window, edge);
        }

        // Draw closing edge after obstacle is finalized
        std::vector<Polygon> obstacles;
        if (obstacleFinalized)
        {
            obstacles.push_back(obstacle);
            Segment closingEdge{
                obstacle.vertices.back(),
                obstacle.vertices.front()
            };

            drawSegment(window, closingEdge);
        }

        // Draw path waypoints
        for (const Point& waypoint : path.waypoints)
        {
            drawPoint(window, waypoint);
        }

        // Draw consecutive path segments
        for (
            std::size_t i = 0;
            i + 1 < path.waypoints.size();
            i++
        )
        {
            Segment pathSegment{
                path.waypoints[i],
                path.waypoints[i + 1]
            };
            bool collisionFree =
                isSegmentCollisionFree(
                    pathSegment,
                    obstacles
                );

            sf::Color segmentColor =
                collisionFree
                    ? sf::Color::Green
                    : sf::Color::Red;

            drawSegment(window, pathSegment, segmentColor);
        }

        window.display();
    }

    return 0;
}