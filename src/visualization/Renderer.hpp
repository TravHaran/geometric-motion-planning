#pragma once

#include <SFML/Graphics.hpp>
#include "../geometry/Point.hpp"
#include "../geometry/Segment.hpp"
#include "../graph/Graph.hpp"

void drawPoint(
    sf::RenderWindow& window,
    const Point& point,
    const sf::Color& color = sf::Color::White
);

void drawSegment(
    sf::RenderWindow& window,
    const Segment& segment,
    const sf::Color& color = sf::Color::White
);

void drawGraph(
    sf::RenderWindow& window,
    const Graph& graph,
    const sf::Color& color = sf::Color::White
);