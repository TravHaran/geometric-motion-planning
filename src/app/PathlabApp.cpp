#include "PathlabApp.hpp"

#include "../visualization/Renderer.hpp"

#include <optional>
#include <chrono>
#include <cmath>

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
    for(const Polygon& obstacle : obstacles){
        const std::size_t vertexCount = obstacle.vertices.size();

        for(std::size_t i = 0; i < vertexCount; ++i){

            const std::size_t nextIndex = (i+1) % vertexCount;

            Segment edge{
                obstacle.vertices[i],
                obstacle.vertices[nextIndex]
            };

            drawSegment(window, edge, sf::Color(180, 185, 195));
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
        drawPoint(window, *start, sf::Color::Green);
    }

    if(goal.has_value()){
        drawPoint(window, *goal, sf::Color::Red);
    }
}

void PathlabApp::drawVisibilityGraph(){
    if(!planningResultAvailable || !showVisibilityGraph) return;

    drawGraph(window, graph, sf::Color(85, 90, 100));

    for(const GraphNode& node : graph.nodes){
        drawPoint(window, node.position, sf::Color(175, 180, 190));
    }
}

void PathlabApp::drawPath(){
    if(!planningResultAvailable) return;

    for(std::size_t i = 0; i+1 < result.path.size(); ++i){
        const std::size_t fromNode = result.path[i];
        const std::size_t toNode = result.path[i+1];

        const Point& from = graph.nodes[fromNode].position;
        const Point& to = graph.nodes[toNode].position;

        Segment pathSegment{from, to};

        drawSegment(window, pathSegment, sf::Color(245, 205, 65));
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