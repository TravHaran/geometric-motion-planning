#pragma once

#include <SFML/Graphics.hpp>
#include "../geometry/Point.hpp"
#include "../geometry/Segment.hpp"

void drawPoint(
    sf::RenderWindow& window,
    const Point& point
);

void drawSegment(
    sf::RenderWindow& window,
    const Segment& segment,
    const sf::Color& color = sf::Color::White
);