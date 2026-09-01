#include "Renderer.hpp"

void drawPoint(
    sf::RenderTarget& target,
    const Point& point,
    const sf::Color& color
){
    sf::CircleShape circle(5.0f);

    circle.setOrigin({5.0f, 5.0f});

    circle.setPosition({
        static_cast<float>(point.x),
        static_cast<float>(point.y)
    });

    circle.setFillColor(color);

    target.draw(circle);
}

void drawSegment(
    sf::RenderTarget& target,
    const Segment& segment,
    const sf::Color& color
) {
   sf::Vertex line[] = {
        sf::Vertex{
            sf::Vector2f{
                static_cast<float>(segment.a.x),
                static_cast<float>(segment.a.y)
            },
            color
        },
        sf::Vertex{
            sf::Vector2f{
                static_cast<float>(segment.b.x),
                static_cast<float>(segment.b.y)
            },
            color
        }
    }; 

    target.draw(line, 2, sf::PrimitiveType::Lines);
}

void drawGraph(
    sf::RenderTarget& target,
    const Graph& graph,
    const sf::Color& color
){
    for(const GraphEdge& edge : graph.edges){

        const Point& a =
            graph.nodes[edge.from].position;

        const Point& b =
            graph.nodes[edge.to].position;

        Segment segment{a, b};

        drawSegment(
            target,
            segment,
            color
        );
    }
}
