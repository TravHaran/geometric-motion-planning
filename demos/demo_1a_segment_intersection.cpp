#include <SFML/Graphics.hpp>

#include <iostream>
#include <optional>
#include <vector>

#include "geometry/Geometry.hpp"
#include "geometry/Point.hpp"
#include "geometry/Segment.hpp"
#include "visualization/Renderer.hpp"

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "Geometric Motion Planning"
    );
    bool segmentsIntersectResult = false;
    std::vector<Point> points;

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

            // Left mouse click
            if (const auto* mouseButtonPressed =
                    event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (
                    mouseButtonPressed->button == sf::Mouse::Button::Left &&
                    points.size() < 4
                )
                {
                    Point clickedPoint{
                        static_cast<double>(mouseButtonPressed->position.x),
                        static_cast<double>(mouseButtonPressed->position.y)
                    };

                    points.push_back(clickedPoint);

                    std::cout
                        << "Point added: ("
                        << clickedPoint.x
                        << ", "
                        << clickedPoint.y
                        << ")\n";

                    // Once we have 4 points, test the two segments
                    if (points.size() == 4)
                    {
                        Segment firstSegment{
                            points[0],
                            points[1]
                        };

                        Segment secondSegment{
                            points[2],
                            points[3]
                        };

                        segmentsIntersectResult =
                            segmentsIntersect(
                                firstSegment,
                                secondSegment
                            );

                        std::cout
                            << std::boolalpha
                            << "Segments intersect: "
                            << segmentsIntersectResult
                            << "\n";
                    }
                }
            }

            // Press R to reset
            if (const auto* keyPressed =
                    event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->code == sf::Keyboard::Key::R)
                {
                    points.clear();
                    segmentsIntersectResult = false;
                    std::cout << "Points cleared.\n";
                }
            }
        }

        // -------------------------
        // Draw everything
        // -------------------------
        window.clear();

        // Draw clicked points
        for (const Point& point : points)
        {
            drawPoint(window, point);
        }

        // Draw first segment: A -> B
        if (points.size() >= 2 && points.size() < 4)
        {
            Segment firstSegment{
                points[0],
                points[1]
            };

            drawSegment(window, firstSegment);
        }

        // Draw second segment: C -> D
        if (points.size() == 4)
        {
            Segment firstSegment{
                points[0],
                points[1]
            };

            Segment secondSegment{
                points[2],
                points[3]
            };

            sf::Color segmentColor =
                segmentsIntersectResult
                    ? sf::Color::Red
                    : sf::Color::Green;

            drawSegment(window, firstSegment, segmentColor);
            drawSegment(window, secondSegment, segmentColor);
        }

        window.display();
    }

    return 0;
}
