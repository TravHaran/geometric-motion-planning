#pragma once

#include <SFML/Graphics.hpp>
#include "../geometry/Point.hpp"
#include "../geometry/Segment.hpp"
#include "../graph/Graph.hpp"

void drawPoint(
    sf::RenderTarget& target,
    const Point& point,
    const sf::Color& color = sf::Color::White
);

void drawSegment(
    sf::RenderTarget& target,
    const Segment& segment,
    const sf::Color& color = sf::Color::White
);

void drawGraph(
    sf::RenderTarget& target,
    const Graph& graph,
    const sf::Color& color = sf::Color::White
);
