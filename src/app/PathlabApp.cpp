#include "PathlabApp.hpp"

#include "../visualization/Renderer.hpp"
#include "../geometry/Triangulation.hpp"

#include <optional>
#include <chrono>
#include <cmath>

namespace{


// bool isConvexForRendering(
//     const Polygon& polygon
// ){
//     if(polygon.vertices.size() < 3) return false;


//     double previousCross = 0.0;


//     const std::size_t n = polygon.vertices.size();


//     for(std::size_t i = 0; i < n; ++i){
//         const Point& a = polygon.vertices[i];

//         const Point& b = polygon.vertices[(i + 1) % n];

//         const Point& c = polygon.vertices[(i + 2) % n];

//         const double cross = 
//             (b.x - a.x) * (c.y - b.y) - (b.y - a.y) * (c.x - b.x);

//         if(std::abs(cross) < 1e-9){
//             continue;
//         }

//         if(previousCross == 0.0){
//             previousCross = cross;
//             continue;
//         }

//         if((cross > 0.0) != (previousCross > 0.0)){
//             return false;
//         }
//     }

//     return true;
// }

void drawThickSegment(
    sf::RenderWindow& window,
    const Point& a,
    const Point& b,
    float thickness,
    const sf::Color& color
){
    const sf::Vector2f start{
        static_cast<float>(a.x), static_cast<float>(a.y)
    };


    const sf::Vector2f end{
        static_cast<float>(b.x), static_cast<float>(b.y)
    };

    const sf::Vector2f direction =
        end - start;

    const float length =
        std::sqrt(
            direction.x * direction.x
            +
            direction.y * direction.y
        );

    if(length == 0.0f){
        return;
    }

    const sf::Vector2f perpendicular{
        -direction.y / length,
        direction.x / length
    };

    const sf::Vector2f offset = perpendicular * (thickness / 2.0f);

    sf::VertexArray strip(sf::PrimitiveType::TriangleStrip);

    strip.append(sf::Vertex{start + offset, color});

    strip.append(sf::Vertex{start - offset, color});

    strip.append(sf::Vertex{end + offset, color});

    strip.append(sf::Vertex{end - offset, color});

    window.draw(strip);
}

void drawEndpointMarker(
    sf::RenderWindow& window,
    const sf::Font& font,
    const Point& point,
    const std::string& letter,
    const std::string& label,
    const sf::Color& color
){
    constexpr float radius = 11.0f;

    // ---------------------------------
    // Outer glow / ring
    // ---------------------------------

    sf::CircleShape outer(radius + 3.0f);

    outer.setOrigin(sf::Vector2f(radius + 3.0f, radius + 3.0f));

    outer.setPosition(
        sf::Vector2f(
            static_cast<float>(point.x),
            static_cast<float>(point.y)
        )
    );

    outer.setFillColor(sf::Color(color.r, color.g, color.b, 45));

    window.draw(outer);


    // ---------------------------------
    // Main marker
    // ---------------------------------

    sf::CircleShape marker(radius);

    marker.setOrigin(sf::Vector2f(radius, radius));

    marker.setPosition(
        sf::Vector2f(
            static_cast<float>(point.x),
            static_cast<float>(point.y)
        )
    );

    marker.setFillColor(color);

    window.draw(marker);


    // ---------------------------------
    // S / G
    // ---------------------------------

    sf::Text markerText(font);

    markerText.setString(letter);

    markerText.setCharacterSize(12);

    markerText.setFillColor(sf::Color::White);


    const sf::FloatRect bounds = markerText.getLocalBounds();


    markerText.setPosition(
        sf::Vector2f(
            static_cast<float>(point.x) - bounds.size.x / 2.0f,
            static_cast<float>(point.y) - bounds.size.y / 2.0f - 3.0f
        )
    );

    window.draw(markerText);

    // ---------------------------------
    // Label
    // ---------------------------------

    sf::Text labelText(font);

    labelText.setString(label);

    labelText.setCharacterSize(11);

    labelText.setFillColor(color);

    labelText.setPosition(
        sf::Vector2f(
            static_cast<float>(point.x) + 17.0f,
            static_cast<float>(point.y) - 8.0f
        )
    );

    window.draw(labelText);
}

}

PathlabApp::PathlabApp() : window(
    sf::VideoMode({1200, 800}),
    "PATHLAB - Path Planning Visualizer"
){
    window.setMinimumSize(sf::Vector2u{900, 650});

    if(!loadPathlabFont(uiFont)) window.close();
}

void PathlabApp::run(){
    while(window.isOpen()){
        processEvents();
        render();
    }
}

void PathlabApp::processEvents(){
    while(const std::optional event = window.pollEvent()){
        if(event->is<sf::Event::Closed>()){
            window.close();
        }


        if(const auto* resized = event->getIf<sf::Event::Resized>()){
            sf::FloatRect visibleArea({0.0f, 0.0f}, sf::Vector2f(resized->size));
            window.setView(sf::View(visibleArea));
        }

        if(const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()){
            handleMousePressed(*mousePressed);
        }

        if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()){
            handleKeyPressed(*keyPressed);
        }
    }
}

void PathlabApp::handleMousePressed( 
    const sf::Event::MouseButtonPressed& event
){
    if(event.button != sf::Mouse::Button::Left) return;

    if(!isInsideCanvas(event.position)) return;

    Point clickedPoint{
        static_cast<double>(event.position.x),
        static_cast<double>(event.position.y)
    };

    if(inputMode == InputMode::Obstacle){
        currentObstacleVertices.push_back(clickedPoint);
    } else if(inputMode == InputMode::Start){
        start = clickedPoint;
        invalidatePlanningResult();
        inputMode = InputMode::Obstacle;
    } else if(inputMode == InputMode::Goal){
        goal = clickedPoint;
        invalidatePlanningResult();
        inputMode = InputMode::Obstacle;
    }
}

void PathlabApp::handleKeyPressed(
    const sf::Event::KeyPressed& event
){
    // Finalize obstacle
    if(event.code == sf::Keyboard::Key::Enter){
        if(currentObstacleVertices.size()>=3){
            Polygon obstacle{currentObstacleVertices};
            obstacles.push_back(obstacle);

            currentObstacleVertices.clear();

            invalidatePlanningResult();
        }
    }
    // Start placement mode
    else if(event.code == sf::Keyboard::Key::S){
        inputMode = InputMode::Start;
    }
    // Goal placement mode
    else if(event.code == sf::Keyboard::Key::G){
        inputMode = InputMode::Goal;
    }
    // Cancel
    else if(event.code == sf::Keyboard::Key::Escape){
        if(inputMode != InputMode::Obstacle){
            inputMode = InputMode::Obstacle;
        } else {
            currentObstacleVertices.clear();
        }
    }
    // Reset scene
    else if(event.code == sf::Keyboard::Key::R){
        resetScene();
    }
    // Toggle visibility graph
    else if(event.code == sf::Keyboard::Key::V){
        showVisibilityGraph = !showVisibilityGraph;
    }
    // Run planner
    else if(event.code == sf::Keyboard::Key::Space){
        runPlanner();
    }
}

void PathlabApp::resetScene(){
    obstacles.clear();
    currentObstacleVertices.clear();
    start.reset();
    goal.reset();
    inputMode = InputMode::Obstacle;
    invalidatePlanningResult();
}

void PathlabApp::render(){
    window.clear(sf::Color(18, 20, 24));

    drawCanvasBackground();

    drawVisibilityGraph();

    drawObstacles();

    drawPath();

    drawCurrentObstacle();

    drawStartAndGoal();

    const PathlabUIData uiData = buildUIData();

    drawPathlabUI(window, uiFont, uiData);

    window.display();
}

void PathlabApp::drawObstacles(){
    const sf::Color obstacleFill{35, 39,47};

    const sf::Color obstacleBorder{112, 119, 132};

    const sf::Color vertexColor{165, 171, 182};

    for(const Polygon& obstacle : obstacles){
        const std::size_t n = obstacle.vertices.size();
        if(n < 3) continue;

        // -----------------------------------
        // Fill obstacles using triangulation
        // -----------------------------------

        const std::vector<Triangle> triangles = triangulatePolygon(obstacle);

        for(const Triangle& triangle : triangles){
            sf::VertexArray fill(sf::PrimitiveType::Triangles);
            fill.append(
                sf::Vertex{
                    sf::Vector2f(
                        static_cast<float>(triangle.a.x), 
                        static_cast<float>(triangle.a.y)),
                        obstacleFill
                }
            );

            fill.append(
                sf::Vertex{
                    sf::Vector2f(
                        static_cast<float>(triangle.b.x), 
                        static_cast<float>(triangle.b.y)),
                        obstacleFill
                }
            );

            fill.append(
                sf::Vertex{
                    sf::Vector2f(
                        static_cast<float>(triangle.c.x), 
                        static_cast<float>(triangle.c.y)),
                        obstacleFill
                }
            );

            window.draw(fill);
        }


        // ---------------------------------
        // Border
        // ---------------------------------

        for(std::size_t i = 0; i < n;++i){

            Segment edge{
                obstacle.vertices[i], 
                obstacle.vertices[(i + 1) % n]
            };

            drawSegment(window, edge, obstacleBorder);
        }


        // ---------------------------------
        // Vertex markers
        // ---------------------------------

        for(const Point& vertex : obstacle.vertices){

            sf::CircleShape marker(2.5f);

            marker.setOrigin(sf::Vector2f(2.5f, 2.5f));

            marker.setPosition(
                sf::Vector2f(
                    static_cast<float>(
                        vertex.x
                    ),
                    static_cast<float>(
                        vertex.y
                    )
                )
            );

            marker.setFillColor(vertexColor);

            window.draw(marker);
        }
    }
}

void PathlabApp::drawCurrentObstacle(){
    // draw edges already entered
    for(std::size_t i = 0; i +1 < currentObstacleVertices.size(); ++i){
        Segment edge{
            currentObstacleVertices[i],
            currentObstacleVertices[i+1]
        };

        drawSegment(window, edge, sf::Color::White);
    }
    // draw obstacle vertices
    for(const Point& vertex : currentObstacleVertices){
        drawPoint(window, vertex, sf::Color::White);
    }
}

void PathlabApp::drawStartAndGoal(){
    if(start.has_value()){
        drawEndpointMarker(
            window,
            uiFont,
            *start,
            "S",
            "Start",
            sf::Color(
                74,
                222,
                128
            )
        );
    }

    if(goal.has_value()){
        drawEndpointMarker(
            window,
            uiFont,
            *goal,
            "G",
            "Goal",
            sf::Color(
                248,
                113,
                113
            )
        );
    }
}

void PathlabApp::drawVisibilityGraph(){
    if(!planningResultAvailable || !showVisibilityGraph) return;

    const sf::Color graphColor{
        62,
        68,
        80
    };

    drawGraph(window, graph, graphColor);

    const sf::Color nodeColor{
        118,
        124,
        136
    };

    for(const GraphNode& node : graph.nodes){
        sf::CircleShape marker(2.5f);
        marker.setOrigin(sf::Vector2f(2.5f, 2.5f));
        marker.setPosition(
            sf::Vector2f(
                static_cast<float>(node.position.x), 
                static_cast<float>(node.position.y) 
            )
        );
        marker.setFillColor(nodeColor);
        window.draw(marker);
    }
}

void PathlabApp::drawPath(){
    if(!planningResultAvailable) return;

    const sf::Color pathColor{250, 204, 21};


    for(std::size_t i = 0; i+1 < result.path.size(); ++i){
        const std::size_t fromNode = result.path[i];
        const std::size_t toNode = result.path[i+1];

        const Point& a = graph.nodes[fromNode].position;
        const Point& b = graph.nodes[toNode].position;

        drawThickSegment(
            window,
            a,
            b,
            4.0f,
            pathColor
        );
    }

    // ---------------------------------
    // Path waypoint markers
    // ---------------------------------

    for(const std::size_t nodeIndex : result.path){

        const Point& point = graph.nodes[nodeIndex].position;

        sf::CircleShape waypoint(3.0f);

        waypoint.setOrigin(sf::Vector2f(3.0f, 3.0f));

        waypoint.setPosition(
            sf::Vector2f(
                static_cast<float>(point.x),
                static_cast<float>(point.y)
            )
        );

        waypoint.setFillColor(sf::Color(255, 238, 150));

        window.draw(waypoint);
    }

}

void PathlabApp::runPlanner(){
    if(!start.has_value() || !goal.has_value()) return;

    // Build visibility graph
    const auto graphBuildStart = std::chrono::steady_clock::now();

    graph = buildVisibilityGraph(*start, *goal, obstacles);

    const auto graphBuildEnd = std::chrono::steady_clock::now();

    graphBuildTimeMs = std::chrono::duration<double, std::milli>(
        graphBuildEnd - graphBuildStart
    ).count();

    // run shortest-path search
    const auto searchStart = std::chrono::steady_clock::now();

    result = dijkstra(graph, 0, 1);

    const auto searchEnd = std::chrono::steady_clock::now();

    searchTimeMs = std::chrono::duration<double, std::milli>(
        searchEnd - searchStart
    ).count();

    planningResultAvailable = true;
}

void PathlabApp::invalidatePlanningResult(){
    graph.nodes.clear();
    graph.edges.clear();
    result.path.clear();

    graphBuildTimeMs = 0.0;
    searchTimeMs = 0.0;
    planningResultAvailable = false;
}

bool PathlabApp::isInsideCanvas(const sf::Vector2i& position) const {
    const sf::Vector2u windowSize = window.getSize();

    const float canvasRight = static_cast<float>(windowSize.x) - PATHLAB_SIDE_PANEL_WIDTH;
    const float canvasBottom = static_cast<float>(windowSize.y) - PATHLAB_BOTTOM_BAR_HEIGHT;

    return 
        position.x >= 0 &&
        position.x < static_cast<int>(canvasRight) &&
        position.y >= static_cast<int>(PATHLAB_TOP_BAR_HEIGHT) &&
        position.y < static_cast<int>(canvasBottom);
}

PathlabUIData PathlabApp::buildUIData() const
{
    PathlabUIData data;

    data.algorithm = "Dijkstra";

    if(!planningResultAvailable){
        data.plannerStatus = "Not Run";
    }
    else if(result.path.empty()){
        data.plannerStatus = "No Path";
    }
    else{
        data.plannerStatus = "Path Found";
    }

    data.obstacleCount = obstacles.size();
    data.graphNodeCount = graph.nodes.size();
    data.graphEdgeCount = graph.edges.size();

    if(planningResultAvailable && std::isfinite(result.distance)){
        data.pathLength = result.distance;
    }

    if(!result.path.empty()){
        data.pathNodes = result.path.size();
        data.pathSegments = result.path.size() - 1;
    }

    data.graphBuildTimeMs = graphBuildTimeMs;
    data.searchTimeMs = searchTimeMs;
    data.totalTimeMs = graphBuildTimeMs + searchTimeMs;
    data.showVisibilityGraph = showVisibilityGraph;

    return data;
}

void PathlabApp::drawCanvasBackground()
{
    const sf::Vector2u windowSize = window.getSize();

    const float canvasWidth = static_cast<float>(windowSize.x) - PATHLAB_SIDE_PANEL_WIDTH;

    const float canvasHeight = 
        static_cast<float>(windowSize.y) - PATHLAB_TOP_BAR_HEIGHT - PATHLAB_BOTTOM_BAR_HEIGHT;

    // ---------------------------------
    // Canvas surface
    // ---------------------------------

    sf::RectangleShape background(sf::Vector2f(canvasWidth, canvasHeight));

    background.setPosition(sf::Vector2f(0.0f, PATHLAB_TOP_BAR_HEIGHT));

    background.setFillColor(sf::Color(16, 18, 22));

    window.draw(background);

    // ---------------------------------
    // Engineering grid
    // ---------------------------------

    const float gridSpacing = 40.0f;

    const sf::Color gridColor{30, 33, 40};


    sf::VertexArray grid(sf::PrimitiveType::Lines);


    for(float x = 0.0f; x <= canvasWidth; x += gridSpacing){
        grid.append(
            sf::Vertex{sf::Vector2f(x, PATHLAB_TOP_BAR_HEIGHT), gridColor}
        );

        grid.append(
            sf::Vertex{sf::Vector2f(x, PATHLAB_TOP_BAR_HEIGHT+ canvasHeight),
                gridColor
            }
        );
    }


    for(float y = PATHLAB_TOP_BAR_HEIGHT; y <= PATHLAB_TOP_BAR_HEIGHT + canvasHeight; y += gridSpacing){

        grid.append(sf::Vertex{sf::Vector2f(0.0f, y), gridColor});

        grid.append(
            sf::Vertex{sf::Vector2f(canvasWidth, y), gridColor}
        );
    }


    window.draw(grid);
}