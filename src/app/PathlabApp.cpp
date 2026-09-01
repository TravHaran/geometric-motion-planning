#include "PathlabApp.hpp"

#include "../visualization/Renderer.hpp"
#include "../geometry/Triangulation.hpp"

#include <optional>
#include <chrono>
#include <cmath>

namespace{

sf::Vector2f toVector(const Point& point){
    return {
        static_cast<float>(point.x),
        static_cast<float>(point.y)
    };
}

void appendLine(
    sf::VertexArray& vertices,
    const Point& a,
    const Point& b,
    const sf::Color& color
){
    vertices.append(sf::Vertex{toVector(a), color});
    vertices.append(sf::Vertex{toVector(b), color});
}

void appendTriangle(
    sf::VertexArray& vertices,
    const Triangle& triangle,
    const sf::Color& color
){
    vertices.append(sf::Vertex{toVector(triangle.a), color});
    vertices.append(sf::Vertex{toVector(triangle.b), color});
    vertices.append(sf::Vertex{toVector(triangle.c), color});
}

void appendFilledCircle(
    sf::VertexArray& vertices,
    const Point& center,
    float radius,
    const sf::Color& color
){
    constexpr std::size_t segmentCount = 10;
    constexpr double twoPi = 6.283185307179586;

    const sf::Vector2f centerPosition = toVector(center);

    for(std::size_t i = 0; i < segmentCount; ++i){
        const double firstAngle = twoPi * static_cast<double>(i) / segmentCount;
        const double secondAngle = twoPi * static_cast<double>(i + 1) / segmentCount;

        const sf::Vector2f firstPoint{
            centerPosition.x + radius * static_cast<float>(std::cos(firstAngle)),
            centerPosition.y + radius * static_cast<float>(std::sin(firstAngle))
        };

        const sf::Vector2f secondPoint{
            centerPosition.x + radius * static_cast<float>(std::cos(secondAngle)),
            centerPosition.y + radius * static_cast<float>(std::sin(secondAngle))
        };

        vertices.append(sf::Vertex{centerPosition, color});
        vertices.append(sf::Vertex{firstPoint, color});
        vertices.append(sf::Vertex{secondPoint, color});
    }
}

void appendThickSegment(
    sf::VertexArray& vertices,
    const Point& a,
    const Point& b,
    float thickness,
    const sf::Color& color
){
    const sf::Vector2f start = toVector(a);
    const sf::Vector2f end = toVector(b);

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

    vertices.append(sf::Vertex{start + offset, color});
    vertices.append(sf::Vertex{start - offset, color});
    vertices.append(sf::Vertex{end + offset, color});

    vertices.append(sf::Vertex{end + offset, color});
    vertices.append(sf::Vertex{start - offset, color});
    vertices.append(sf::Vertex{end - offset, color});
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
    window.setFramerateLimit(60);
    window.setMinimumSize(sf::Vector2u{900, 650});

    rebuildCanvasRenderCache();

    if(!loadPathlabFont(uiFont)) window.close();
}

void PathlabApp::run(){
    render();

    while(window.isOpen()){
        processEvents();

        if(window.isOpen()){
            render();
        }
    }
}

void PathlabApp::processEvents(){
    // PATHLAB has no animation, so wait instead of continuously redrawing
    // an unchanged scene. The frame cap still protects against event floods.
    if(const std::optional event = window.waitEvent()){
        processEvent(*event);
    }

    while(const std::optional event = window.pollEvent()){
        processEvent(*event);
    }
}

void PathlabApp::processEvent(const sf::Event& event){
    if(event.is<sf::Event::Closed>()){
        window.close();
    }

    if(const auto* resized = event.getIf<sf::Event::Resized>()){
        sf::FloatRect visibleArea({0.0f, 0.0f}, sf::Vector2f(resized->size));
        window.setView(sf::View(visibleArea));
        rebuildCanvasRenderCache();
    }

    if(const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()){
        handleMousePressed(*mousePressed);
    }

    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        handleKeyPressed(*keyPressed);
    }
}

void PathlabApp::handleMousePressed( 
    const sf::Event::MouseButtonPressed& event
){
    if(event.button != sf::Mouse::Button::Left) return;

    // =====================================
    // UI interaction
    // =====================================

    const PathlabUIAction uiAction =
        handlePathlabUIClick(
            event.position,
            window.getSize(),
            algorithmDropdownOpen
        );

    switch(uiAction){

        case PathlabUIAction::RunPlanner:

            algorithmDropdownOpen =
                false;

            runPlanner();

            return;


        case PathlabUIAction::ToggleObstacles:

            algorithmDropdownOpen =
                false;

            showObstacles =
                !showObstacles;

            return;


        case PathlabUIAction::ToggleVisibilityGraph:

            algorithmDropdownOpen =
                false;

            showVisibilityGraph =
                !showVisibilityGraph;

            return;


        case PathlabUIAction::ToggleFinalPath:

            algorithmDropdownOpen =
                false;

            showFinalPath =
                !showFinalPath;

            return;


        case PathlabUIAction::ToggleAlgorithmDropdown:

            algorithmDropdownOpen =
                !algorithmDropdownOpen;

            return;


        case PathlabUIAction::SelectDijkstra:

            algorithmDropdownOpen =
                false;


            if(selectedPlanner !=
            PlannerType::Dijkstra){

                selectedPlanner =
                    PlannerType::Dijkstra;

                invalidatePlanningResult();
            }

            return;


        case PathlabUIAction::SelectAStar:

            algorithmDropdownOpen =
                false;


            if(selectedPlanner !=
            PlannerType::AStar){

                selectedPlanner =
                    PlannerType::AStar;

                invalidatePlanningResult();
            }

            return;


        case PathlabUIAction::CloseAlgorithmDropdown:

            algorithmDropdownOpen =
                false;

            return;


        case PathlabUIAction::None:

            break;
    }

    // =====================================
    // Canvas interaction
    // =====================================

    if(!isInsideCanvas(event.position)){
        return;
    }

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
    if(
        event.code == sf::Keyboard::Key::Escape && algorithmDropdownOpen
    ){
        algorithmDropdownOpen = false;

        return;
    }
    // Finalize obstacle
    if(event.code == sf::Keyboard::Key::Enter){
        if(currentObstacleVertices.size()>=3){
            Polygon obstacle{currentObstacleVertices};
            obstacles.push_back(obstacle);

            currentObstacleVertices.clear();

            rebuildObstacleRenderCache();
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
    rebuildObstacleRenderCache();
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
    if(!showObstacles){
        return;
    }

    window.draw(obstacleFillVertices);
    window.draw(obstacleBorderVertices);
    window.draw(obstacleMarkerVertices);
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

    window.draw(visibilityGraphVertices);
    window.draw(visibilityNodeVertices);
}

void PathlabApp::drawPath(){
    if(!planningResultAvailable || !showFinalPath) return;

    window.draw(pathVertices);
    window.draw(pathMarkerVertices);
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

    if(selectedPlanner == PlannerType::Dijkstra){
        const DijkstraResult plannerResult =
            dijkstra(
                graph,
                0,
                1
            );

        result.distance =
            plannerResult.distance;

        result.path =
            plannerResult.path;

        result.expandedNodes =
            plannerResult.expandedNodes;
    } else{
        const AStarResult plannerResult =
            aStar(
                graph,
                0,
                1
            );

        result.distance =
            plannerResult.distance;

        result.path =
            plannerResult.path;

        result.expandedNodes =
            plannerResult.expandedNodes;
    }

    const auto searchEnd = std::chrono::steady_clock::now();

    searchTimeMs = std::chrono::duration<double, std::milli>(
        searchEnd - searchStart
    ).count();

    planningResultAvailable = true;
    rebuildPlanningRenderCache();
}

void PathlabApp::invalidatePlanningResult(){
    graph.nodes.clear();
    graph.edges.clear();
    result.path.clear();

    visibilityGraphVertices.clear();
    visibilityNodeVertices.clear();
    pathVertices.clear();
    pathMarkerVertices.clear();

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

    if(selectedPlanner == PlannerType::Dijkstra){
        data.algorithm = "Dijkstra";
    } else{
        data.algorithm = "A*";
    }

    data.algorithmDropdownOpen = algorithmDropdownOpen;

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

    data.nodesExpanded = planningResultAvailable ? result.expandedNodes : 0;

    data.graphBuildTimeMs = graphBuildTimeMs;
    data.searchTimeMs = searchTimeMs;
    data.totalTimeMs = graphBuildTimeMs + searchTimeMs;
    data.showObstacles = showObstacles;
    data.showVisibilityGraph = showVisibilityGraph;
    data.showFinalPath = showFinalPath;

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

    window.draw(canvasGridVertices);
}

void PathlabApp::rebuildCanvasRenderCache(){
    canvasGridVertices.clear();

    const sf::Vector2u windowSize = window.getSize();
    const float canvasWidth = static_cast<float>(windowSize.x) - PATHLAB_SIDE_PANEL_WIDTH;
    const float canvasHeight =
        static_cast<float>(windowSize.y) - PATHLAB_TOP_BAR_HEIGHT - PATHLAB_BOTTOM_BAR_HEIGHT;

    constexpr float gridSpacing = 40.0f;
    const sf::Color gridColor{30, 33, 40};

    for(float x = 0.0f; x <= canvasWidth; x += gridSpacing){
        canvasGridVertices.append(
            sf::Vertex{sf::Vector2f(x, PATHLAB_TOP_BAR_HEIGHT), gridColor}
        );
        canvasGridVertices.append(
            sf::Vertex{sf::Vector2f(x, PATHLAB_TOP_BAR_HEIGHT + canvasHeight), gridColor}
        );
    }

    for(float y = PATHLAB_TOP_BAR_HEIGHT;
        y <= PATHLAB_TOP_BAR_HEIGHT + canvasHeight;
        y += gridSpacing){
        canvasGridVertices.append(sf::Vertex{sf::Vector2f(0.0f, y), gridColor});
        canvasGridVertices.append(sf::Vertex{sf::Vector2f(canvasWidth, y), gridColor});
    }
}

void PathlabApp::rebuildObstacleRenderCache(){
    obstacleFillVertices.clear();
    obstacleBorderVertices.clear();
    obstacleMarkerVertices.clear();

    const sf::Color obstacleFill{35, 39, 47};
    const sf::Color obstacleBorder{112, 119, 132};
    const sf::Color vertexColor{165, 171, 182};

    for(const Polygon& obstacle : obstacles){
        const std::size_t vertexCount = obstacle.vertices.size();
        if(vertexCount < 3) continue;

        for(const Triangle& triangle : triangulatePolygon(obstacle)){
            appendTriangle(obstacleFillVertices, triangle, obstacleFill);
        }

        for(std::size_t i = 0; i < vertexCount; ++i){
            appendLine(
                obstacleBorderVertices,
                obstacle.vertices[i],
                obstacle.vertices[(i + 1) % vertexCount],
                obstacleBorder
            );
        }

        for(const Point& vertex : obstacle.vertices){
            appendFilledCircle(obstacleMarkerVertices, vertex, 2.5f, vertexColor);
        }
    }
}

void PathlabApp::rebuildPlanningRenderCache(){
    visibilityGraphVertices.clear();
    visibilityNodeVertices.clear();
    pathVertices.clear();
    pathMarkerVertices.clear();

    const sf::Color graphColor{62, 68, 80};
    const sf::Color nodeColor{118, 124, 136};

    for(const GraphEdge& edge : graph.edges){
        appendLine(
            visibilityGraphVertices,
            graph.nodes[edge.from].position,
            graph.nodes[edge.to].position,
            graphColor
        );
    }

    for(const GraphNode& node : graph.nodes){
        appendFilledCircle(visibilityNodeVertices, node.position, 2.5f, nodeColor);
    }

    const sf::Color pathColor{250, 204, 21};
    for(std::size_t i = 0; i + 1 < result.path.size(); ++i){
        appendThickSegment(
            pathVertices,
            graph.nodes[result.path[i]].position,
            graph.nodes[result.path[i + 1]].position,
            4.0f,
            pathColor
        );
    }

    const sf::Color pathMarkerColor{255, 238, 150};
    for(const std::size_t nodeIndex : result.path){
        appendFilledCircle(
            pathMarkerVertices,
            graph.nodes[nodeIndex].position,
            3.0f,
            pathMarkerColor
        );
    }
}
