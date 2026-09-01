#include "PathlabApp.hpp"

#include "../visualization/Renderer.hpp"
#include "../geometry/Triangulation.hpp"

#include <optional>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <iostream>

namespace{

constexpr float MIN_CANVAS_ZOOM = 0.25f;
constexpr float MAX_CANVAS_ZOOM = 4.0f;
constexpr float CANVAS_ZOOM_STEP = 1.15f;
constexpr float CANVAS_WORLD_HALF_EXTENT = 50000.0f;
constexpr unsigned int GLASS_BLUR_DOWNSAMPLE = 4;

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

void appendRectangle(
    sf::VertexArray& vertices,
    const sf::FloatRect& rectangle,
    const sf::Color& color
){
    const sf::Vector2f topLeft = rectangle.position;
    const sf::Vector2f topRight{
        rectangle.position.x + rectangle.size.x,
        rectangle.position.y
    };
    const sf::Vector2f bottomLeft{
        rectangle.position.x,
        rectangle.position.y + rectangle.size.y
    };
    const sf::Vector2f bottomRight = rectangle.position + rectangle.size;

    vertices.append(sf::Vertex{topLeft, color});
    vertices.append(sf::Vertex{bottomLeft, color});
    vertices.append(sf::Vertex{topRight, color});

    vertices.append(sf::Vertex{topRight, color});
    vertices.append(sf::Vertex{bottomLeft, color});
    vertices.append(sf::Vertex{bottomRight, color});
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
    sf::RenderTarget& target,
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

    target.draw(outer);


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

    target.draw(marker);


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

    target.draw(markerText);

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

    target.draw(labelText);
}

}

PathlabApp::PathlabApp() : window(
    sf::VideoMode({1200, 800}),
    "PATHLAB - Path Planning Visualizer"
){
    window.setFramerateLimit(60);
    window.setMinimumSize(sf::Vector2u{900, 650});

    initializeViews();
    rebuildCanvasRenderCache();

    updatePlaybackInterval();

    if(!loadPathlabFont(uiFont)) window.close();

    if(window.isOpen()){
        initializeGlassBackdrop();
    }
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
    // Outside active playback, wait instead of continuously redrawing an
    // unchanged scene. The frame cap still protects playback and event bursts.

    if(playbackPlaying){

        while(
            const std::optional event =
                window.pollEvent()
        ){
            processEvent(
                *event
            );
        }

        updatePlayback();

        return;
    }

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

    if(event.is<sf::Event::Resized>()){
        updateViewLayout();

        if(glassBlurAvailable && !resizeGlassRenderTargets()){
            glassBlurAvailable = false;

            std::cerr
                << "PATHLAB glass blur disabled after render-target resize failure."
                << std::endl;
        }
    }

    if(const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()){
        handleMousePressed(*mousePressed);
    }

    if(const auto* mouseReleased = event.getIf<sf::Event::MouseButtonReleased>()){
        handleMouseReleased(*mouseReleased);
    }

    if(const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>()){
        handleMouseMoved(*mouseMoved);
    }

    if(const auto* wheelScrolled = event.getIf<sf::Event::MouseWheelScrolled>()){
        handleMouseWheelScrolled(*wheelScrolled);
    }

    if(event.is<sf::Event::FocusLost>()){
        canvasPanning = false;
    }

    if(const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()){
        handleKeyPressed(*keyPressed);
    }
}

void PathlabApp::handleMousePressed( 
    const sf::Event::MouseButtonPressed& event
){
    if(event.button != sf::Mouse::Button::Left) return;

    if(helpOverlayOpen){
        if(isPathlabHelpCloseButtonAt(event.position, window.getSize())){
            helpOverlayOpen = false;
        }

        return;
    }

    // =====================================
    // UI interaction
    // =====================================

    const PathlabUIData uiData = buildUIData();

    const PathlabUIAction uiAction =
        handlePathlabUIClick(
            event.position,
            window.getSize(),
            uiData
        );

    switch(uiAction){

        case PathlabUIAction::OpenHelpOverlay:

            helpOverlayOpen = true;
            algorithmDropdownOpen = false;
            canvasPanning = false;

            return;

        case PathlabUIAction::ToggleSidebar:

            toggleSidebar();

            return;

        case PathlabUIAction::LoadDemoScene:

            algorithmDropdownOpen = false;
            loadDemoScene();

            return;

        case PathlabUIAction::ResetCamera:

            algorithmDropdownOpen = false;
            resetCanvasView();

            return;

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

            worldBackdropDirty = true;

            return;


        case PathlabUIAction::ToggleVisibilityGraph:

            algorithmDropdownOpen =
                false;

            showVisibilityGraph =
                !showVisibilityGraph;

            worldBackdropDirty = true;

            return;


        case PathlabUIAction::ToggleFinalPath:

            algorithmDropdownOpen =
                false;

            showFinalPath =
                !showFinalPath;

            worldBackdropDirty = true;

            return;


        case PathlabUIAction::ToggleExploredNodes:

            algorithmDropdownOpen =
                false;

            showExploredNodes =
                !showExploredNodes;

            worldBackdropDirty = true;

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

        case PathlabUIAction::StepPlayback:

            playbackActive = true;
            playbackPlaying = false;

            if(playbackIndex <
            result.expandedNodeOrder.size()){

                ++playbackIndex;

                rebuildExploredNodeRenderCache();
            }

            rebuildPlaybackHighlightCache();

            return;

        case PathlabUIAction::ResetPlayback:

            playbackActive = true;

            playbackPlaying =
                false;

            playbackIndex = 0;

            rebuildExploredNodeRenderCache();
            rebuildPlaybackHighlightCache();

            return;

        case PathlabUIAction::TogglePlayback:

            playbackActive = true;

            if(playbackIndex >=
            result.expandedNodeOrder.size()){

                playbackIndex = 0;

                rebuildExploredNodeRenderCache();
            }

            rebuildPlaybackHighlightCache();


            playbackPlaying =
                !playbackPlaying;


            if(playbackPlaying){

                playbackClock.restart();
            }

            return;

        case PathlabUIAction::CyclePlaybackSpeed:

            playbackSpeedIndex =
                (playbackSpeedIndex + 1) % 5;

            updatePlaybackInterval();

            if(playbackPlaying){
                playbackClock.restart();
            }

            return;

        case PathlabUIAction::None:

            break;
    }

    if(isPathlabUIOverlayAt(
        event.position,
        window.getSize(),
        uiData.hasSearchTrace,
        uiData.sidebarVisible
    )){
        return;
    }

    // =====================================
    // Canvas interaction
    // =====================================

    if(!isInsideCanvas(event.position)){
        return;
    }

    if(isPanModifierPressed()){
        canvasPanning = true;
        canvasPanButton = sf::Mouse::Button::Left;
        lastPanPixel = event.position;
        return;
    }

    const sf::Vector2f worldPosition =
        window.mapPixelToCoords(event.position, canvasView);

    Point clickedPoint{worldPosition.x, worldPosition.y};

    if(inputMode == InputMode::Obstacle){
        currentObstacleVertices.push_back(clickedPoint);
        worldBackdropDirty = true;
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

void PathlabApp::handleMouseReleased(
    const sf::Event::MouseButtonReleased& event
){
    if(canvasPanning && event.button == canvasPanButton){
        canvasPanning = false;
    }
}

void PathlabApp::handleMouseMoved(
    const sf::Event::MouseMoved& event
){
    if(helpOverlayOpen){
        return;
    }

    if(!canvasPanning){
        return;
    }

    if(
        !sf::Mouse::isButtonPressed(canvasPanButton)
        ||
        !isPanModifierPressed()
    ){
        canvasPanning = false;
        return;
    }

    const sf::Vector2f previousWorldPosition =
        window.mapPixelToCoords(lastPanPixel, canvasView);

    const sf::Vector2f currentWorldPosition =
        window.mapPixelToCoords(event.position, canvasView);

    canvasView.move(previousWorldPosition - currentWorldPosition);
    lastPanPixel = event.position;
    worldBackdropDirty = true;
}

void PathlabApp::handleMouseWheelScrolled(
    const sf::Event::MouseWheelScrolled& event
){
    if(helpOverlayOpen){
        return;
    }

    if(
        event.wheel != sf::Mouse::Wheel::Vertical
        ||
        !isInsideCanvas(event.position)
    ){
        return;
    }

    const sf::Vector2f worldBeforeZoom =
        window.mapPixelToCoords(event.position, canvasView);

    const float requestedZoom =
        canvasZoom * std::pow(CANVAS_ZOOM_STEP, event.delta);

    const float newZoom = std::clamp(
        requestedZoom,
        MIN_CANVAS_ZOOM,
        MAX_CANVAS_ZOOM
    );

    if(newZoom == canvasZoom){
        return;
    }

    canvasZoom = newZoom;
    updateViewLayout();

    const sf::Vector2f worldAfterZoom =
        window.mapPixelToCoords(event.position, canvasView);

    canvasView.move(worldBeforeZoom - worldAfterZoom);
}

void PathlabApp::handleKeyPressed(
    const sf::Event::KeyPressed& event
){
    const bool helpShortcut =
        event.code == sf::Keyboard::Key::Slash
        && event.shift;

    if(helpShortcut){
        helpOverlayOpen = !helpOverlayOpen;
        algorithmDropdownOpen = false;
        canvasPanning = false;

        return;
    }

    if(helpOverlayOpen){
        if(event.code == sf::Keyboard::Key::Escape){
            helpOverlayOpen = false;
        }

        return;
    }

    if(event.code == sf::Keyboard::Key::Tab){
        toggleSidebar();

        return;
    }

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
            worldBackdropDirty = true;
        }
    }
    // Reset scene
    else if(event.code == sf::Keyboard::Key::R){
        resetScene();
    }
    // Toggle visibility graph
    else if(event.code == sf::Keyboard::Key::V){
        if(planningResultAvailable && !graph.nodes.empty()){
            showVisibilityGraph = !showVisibilityGraph;
            worldBackdropDirty = true;
        }
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

void PathlabApp::loadDemoScene(){
    resetScene();

    start = Point{70.0, 410.0};
    goal = Point{830.0, 410.0};

    // Four axis-aligned blocks form a clean rising corridor from start to goal.
    obstacles = {
        Polygon{{
            {185.0, 320.0},
            {285.0, 320.0},
            {285.0, 530.0},
            {185.0, 530.0}
        }},
        Polygon{{
            {345.0, 175.0},
            {435.0, 175.0},
            {435.0, 365.0},
            {345.0, 365.0}
        }},
        Polygon{{
            {505.0, 205.0},
            {580.0, 205.0},
            {580.0, 385.0},
            {505.0, 385.0}
        }},
        Polygon{{
            {665.0, 200.0},
            {745.0, 200.0},
            {745.0, 400.0},
            {665.0, 400.0}
        }}
    };

    rebuildObstacleRenderCache();
    resetCanvasView();
}

void PathlabApp::render(){
    window.clear(sf::Color(18, 20, 24));

    PathlabGlassBackdrop glassBackdrop;

    if(glassBlurAvailable){
        if(worldBackdropDirty){
            updateGlassBackdrop();
        }

        window.setView(uiView);

        sf::Sprite sceneSprite(sceneRenderTexture.getTexture());
        window.draw(sceneSprite);

        glassBackdrop.texture = &verticalBlurTexture.getTexture();
        glassBackdrop.sourceWindowSize = window.getSize();
    } else{
        window.setView(canvasView);
        renderWorld(window);
    }

    window.setView(uiView);

    const PathlabUIData uiData = buildUIData();

    drawPathlabUI(window, uiFont, uiData, glassBackdrop);

    window.display();
}

void PathlabApp::renderWorld(sf::RenderTarget& target){
    drawCanvasBackground(target);

    drawVisibilityGraph(target);

    drawObstacles(target);

    drawExploredNodes(target);

    drawPlaybackHighlights(target);

    drawPath(target);

    drawCurrentObstacle(target);

    drawStartAndGoal(target);
}

void PathlabApp::drawObstacles(sf::RenderTarget& target){
    if(!showObstacles){
        return;
    }

    target.draw(obstacleFillVertices);
    target.draw(obstacleBorderVertices);
    target.draw(obstacleMarkerVertices);
}

void PathlabApp::drawCurrentObstacle(sf::RenderTarget& target){
    // draw edges already entered
    for(std::size_t i = 0; i +1 < currentObstacleVertices.size(); ++i){
        Segment edge{
            currentObstacleVertices[i],
            currentObstacleVertices[i+1]
        };

        drawSegment(target, edge, sf::Color::White);
    }
    // draw obstacle vertices
    for(const Point& vertex : currentObstacleVertices){
        drawPoint(target, vertex, sf::Color::White);
    }
}

void PathlabApp::drawStartAndGoal(sf::RenderTarget& target){
    if(start.has_value()){
        drawEndpointMarker(
            target,
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
            target,
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

void PathlabApp::drawVisibilityGraph(sf::RenderTarget& target){
    if(!planningResultAvailable || !showVisibilityGraph) return;

    target.draw(visibilityGraphVertices);
    target.draw(visibilityNodeVertices);
}

void PathlabApp::drawExploredNodes(sf::RenderTarget& target){

    if(
        !planningResultAvailable
        ||
        !showExploredNodes
    ){
        return;
    }

    target.draw(
        exploredNodeVertices
    );
}

void PathlabApp::drawPlaybackHighlights(sf::RenderTarget& target){
    if(!planningResultAvailable || !playbackActive){
        return;
    }

    if(showExploredNodes){
        target.draw(currentExpandedNodeVertices);
    }

    target.draw(goalReachedVertices);
}

void PathlabApp::drawPath(sf::RenderTarget& target){
    if(!planningResultAvailable || !showFinalPath) return;

    if(
        playbackActive
        &&
        playbackIndex < result.expandedNodeOrder.size()
    ){
        return;
    }

    target.draw(pathVertices);
    target.draw(pathMarkerVertices);
}

bool PathlabApp::canRunPlanner() const {
    return start.has_value() && goal.has_value();
}

void PathlabApp::runPlanner(){
    if(!canRunPlanner()) return;

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
        
        result.expandedNodeOrder =
            plannerResult.expandedNodeOrder;
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
        
        result.expandedNodeOrder =
            plannerResult.expandedNodeOrder;
    }

    const auto searchEnd = std::chrono::steady_clock::now();

    searchTimeMs = std::chrono::duration<double, std::milli>(
        searchEnd - searchStart
    ).count();

    playbackPlaying = false;

    playbackActive = false;

    playbackIndex = result.expandedNodeOrder.size();

    planningResultAvailable = true;

    rebuildPlanningRenderCache();
}

void PathlabApp::invalidatePlanningResult(){
    worldBackdropDirty = true;

    graph.nodes.clear();
    graph.edges.clear();

    result.path.clear();
    result.expandedNodeOrder.clear();

    playbackIndex = 0;
    playbackActive = false;
    playbackPlaying = false;

    result.distance = 0.0;
    result.expandedNodes = 0;

    visibilityGraphVertices.clear();
    visibilityNodeVertices.clear();

    exploredNodeVertices.clear();
    currentExpandedNodeVertices.clear();
    goalReachedVertices.clear();

    pathVertices.clear();
    pathMarkerVertices.clear();

    graphBuildTimeMs = 0.0;
    searchTimeMs = 0.0;

    planningResultAvailable = false;

}

bool PathlabApp::isInsideCanvas(const sf::Vector2i& position) const {
    const sf::Vector2u windowSize = window.getSize();

    const float canvasRight = getUsableCanvasWidth();
    const float canvasBottom = static_cast<float>(windowSize.y) - PATHLAB_BOTTOM_BAR_HEIGHT;

    return 
        position.x >= 0 &&
        position.x < static_cast<int>(canvasRight) &&
        position.y >= static_cast<int>(PATHLAB_TOP_BAR_HEIGHT) &&
        position.y < static_cast<int>(canvasBottom);
}

bool PathlabApp::isPanModifierPressed() const {
    return
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt)
        ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RAlt);
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
    data.helpOverlayOpen = helpOverlayOpen;
    data.sidebarVisible = sidebarVisible;
    data.canRunPlanner = canRunPlanner();

    if(!start.has_value() && !goal.has_value()){
        data.plannerStatus = "Set Start & Goal";
    }
    else if(!start.has_value()){
        data.plannerStatus = "Set Start";
    }
    else if(!goal.has_value()){
        data.plannerStatus = "Set Goal";
    }
    else if(!planningResultAvailable){
        data.plannerStatus = "Ready";
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
    data.hasObstacles = !obstacles.empty();
    data.hasVisibilityGraph =
        planningResultAvailable
        &&
        !graph.nodes.empty();
    data.hasFinalPath =
        planningResultAvailable
        &&
        !result.path.empty();
    data.showExploredNodes = showExploredNodes;
    data.hasSearchTrace =
        planningResultAvailable
        &&
        !result.expandedNodeOrder.empty();
    data.playbackIndex = playbackIndex;
    data.playbackTotal = result.expandedNodeOrder.size();
    data.playbackPlaying = playbackPlaying;

    switch(playbackSpeedIndex){

        case 0:
            data.playbackSpeed = "0.25x";
            break;

        case 1:
            data.playbackSpeed = "0.5x";
            break;

        case 2:
            data.playbackSpeed = "1x";
            break;

        case 3:
            data.playbackSpeed = "2x";
            break;

        case 4:
            data.playbackSpeed = "4x";
            break;

        default:
            data.playbackSpeed = "1x";
            break;
    }

    return data;
}

void PathlabApp::drawCanvasBackground(sf::RenderTarget& target)
{
    target.draw(canvasBackgroundVertices);
    target.draw(canvasGridVertices);
}

void PathlabApp::rebuildCanvasRenderCache(){
    canvasBackgroundVertices.clear();
    canvasGridVertices.clear();

    constexpr float gridSpacing = 40.0f;
    const float worldMinimum = -CANVAS_WORLD_HALF_EXTENT;
    const float worldMaximum = CANVAS_WORLD_HALF_EXTENT;

    appendRectangle(
        canvasBackgroundVertices,
        sf::FloatRect(
            sf::Vector2f(worldMinimum, worldMinimum),
            sf::Vector2f(
                2.0f * CANVAS_WORLD_HALF_EXTENT,
                2.0f * CANVAS_WORLD_HALF_EXTENT
            )
        ),
        sf::Color(16, 18, 22)
    );

    const sf::Color gridColor{30, 33, 40};

    const float firstVerticalLine =
        std::ceil(worldMinimum / gridSpacing) * gridSpacing;

    for(float x = firstVerticalLine; x <= worldMaximum; x += gridSpacing){
        canvasGridVertices.append(
            sf::Vertex{sf::Vector2f(x, worldMinimum), gridColor}
        );
        canvasGridVertices.append(
            sf::Vertex{sf::Vector2f(x, worldMaximum), gridColor}
        );
    }

    const float firstHorizontalLine =
        std::ceil(
            (worldMinimum - PATHLAB_TOP_BAR_HEIGHT)
            / gridSpacing
        )
        * gridSpacing
        + PATHLAB_TOP_BAR_HEIGHT;

    for(float y = firstHorizontalLine; y <= worldMaximum; y += gridSpacing){
        canvasGridVertices.append(
            sf::Vertex{sf::Vector2f(worldMinimum, y), gridColor}
        );
        canvasGridVertices.append(
            sf::Vertex{sf::Vector2f(worldMaximum, y), gridColor}
        );
    }
}

void PathlabApp::initializeViews(){
    resetCanvasView();
}

void PathlabApp::initializeGlassBackdrop(){
    if(!sf::Shader::isAvailable()){
        std::cerr
            << "PATHLAB glass blur unavailable: shaders are not supported."
            << std::endl;

        return;
    }

    if(!gaussianBlurShader.loadFromFile(
        "assets/shaders/gaussian_blur.frag",
        sf::Shader::Type::Fragment
    )){
        std::cerr
            << "PATHLAB glass blur unavailable: blur shader could not be loaded."
            << std::endl;

        return;
    }

    gaussianBlurShader.setUniform(
        "source",
        sf::Shader::CurrentTexture
    );

    if(!resizeGlassRenderTargets()){
        std::cerr
            << "PATHLAB glass blur unavailable: render targets could not be created."
            << std::endl;

        return;
    }

    glassBlurAvailable = true;
    worldBackdropDirty = true;
}

bool PathlabApp::resizeGlassRenderTargets(){
    const sf::Vector2u windowSize = window.getSize();

    const sf::Vector2u blurSize{
        std::max(
            1u,
            (windowSize.x + GLASS_BLUR_DOWNSAMPLE - 1)
                / GLASS_BLUR_DOWNSAMPLE
        ),
        std::max(
            1u,
            (windowSize.y + GLASS_BLUR_DOWNSAMPLE - 1)
                / GLASS_BLUR_DOWNSAMPLE
        )
    };

    const bool sceneReady = sceneRenderTexture.resize(windowSize);
    const bool sourceReady = blurSourceTexture.resize(blurSize);
    const bool horizontalReady = horizontalBlurTexture.resize(blurSize);
    const bool verticalReady = verticalBlurTexture.resize(blurSize);

    if(!sceneReady || !sourceReady || !horizontalReady || !verticalReady){
        return false;
    }

    sceneRenderTexture.setSmooth(true);
    blurSourceTexture.setSmooth(true);
    horizontalBlurTexture.setSmooth(true);
    verticalBlurTexture.setSmooth(true);

    worldBackdropDirty = true;

    return true;
}

sf::View PathlabApp::getBackdropCanvasView() const {
    const sf::Vector2u windowSize = window.getSize();

    const float windowWidth =
        std::max(1.0f, static_cast<float>(windowSize.x));

    const float windowHeight =
        std::max(1.0f, static_cast<float>(windowSize.y));

    const float canvasHeight = std::max(
        1.0f,
        windowHeight
            - PATHLAB_TOP_BAR_HEIGHT
            - PATHLAB_BOTTOM_BAR_HEIGHT
    );

    const float leftWorldEdge =
        canvasView.getCenter().x - canvasView.getSize().x / 2.0f;

    sf::View backdropView = canvasView;

    backdropView.setSize(
        sf::Vector2f(
            windowWidth / canvasZoom,
            canvasHeight / canvasZoom
        )
    );

    backdropView.setCenter(
        sf::Vector2f(
            leftWorldEdge + windowWidth / (2.0f * canvasZoom),
            canvasView.getCenter().y
        )
    );

    backdropView.setViewport(
        sf::FloatRect(
            sf::Vector2f(
                0.0f,
                PATHLAB_TOP_BAR_HEIGHT / windowHeight
            ),
            sf::Vector2f(
                1.0f,
                canvasHeight / windowHeight
            )
        )
    );

    return backdropView;
}

void PathlabApp::updateGlassBackdrop(){
    const sf::Vector2u windowSize = window.getSize();
    const sf::Vector2u blurSize = blurSourceTexture.getSize();

    sceneRenderTexture.clear(sf::Color(18, 20, 24));
    sceneRenderTexture.setView(getBackdropCanvasView());
    renderWorld(sceneRenderTexture);
    sceneRenderTexture.display();

    blurSourceTexture.clear(sf::Color(18, 20, 24));
    blurSourceTexture.setView(blurSourceTexture.getDefaultView());

    sf::Sprite sceneSprite(sceneRenderTexture.getTexture());
    sceneSprite.setScale(
        sf::Vector2f(
            static_cast<float>(blurSize.x)
                / static_cast<float>(windowSize.x),
            static_cast<float>(blurSize.y)
                / static_cast<float>(windowSize.y)
        )
    );

    blurSourceTexture.draw(sceneSprite);
    blurSourceTexture.display();

    horizontalBlurTexture.clear(sf::Color(18, 20, 24));
    horizontalBlurTexture.setView(horizontalBlurTexture.getDefaultView());

    gaussianBlurShader.setUniform(
        "texelStep",
        sf::Glsl::Vec2(1.0f / static_cast<float>(blurSize.x), 0.0f)
    );

    sf::RenderStates blurStates;
    blurStates.shader = &gaussianBlurShader;

    sf::Sprite horizontalSprite(blurSourceTexture.getTexture());
    horizontalBlurTexture.draw(horizontalSprite, blurStates);
    horizontalBlurTexture.display();

    verticalBlurTexture.clear(sf::Color(18, 20, 24));
    verticalBlurTexture.setView(verticalBlurTexture.getDefaultView());

    gaussianBlurShader.setUniform(
        "texelStep",
        sf::Glsl::Vec2(0.0f, 1.0f / static_cast<float>(blurSize.y))
    );

    sf::Sprite verticalSprite(horizontalBlurTexture.getTexture());
    verticalBlurTexture.draw(verticalSprite, blurStates);
    verticalBlurTexture.display();

    worldBackdropDirty = false;
}

void PathlabApp::resetCanvasView(){
    const sf::Vector2u windowSize = window.getSize();

    const float canvasWidth = getUsableCanvasWidth();

    const float canvasHeight = std::max(
        1.0f,
        static_cast<float>(windowSize.y)
            - PATHLAB_TOP_BAR_HEIGHT
            - PATHLAB_BOTTOM_BAR_HEIGHT
    );

    canvasZoom = 1.0f;
    canvasPanning = false;

    canvasView.setCenter(
        sf::Vector2f(
            canvasWidth / 2.0f,
            PATHLAB_TOP_BAR_HEIGHT + canvasHeight / 2.0f
        )
    );

    updateViewLayout();
}

void PathlabApp::toggleSidebar(){
    const float previousCanvasWidth = getUsableCanvasWidth();

    sidebarVisible = !sidebarVisible;
    algorithmDropdownOpen = false;
    canvasPanning = false;

    const float newCanvasWidth = getUsableCanvasWidth();

    // Keep the canvas's left world edge fixed while its screen-space width
    // changes. This preserves the current zoom and avoids shifting existing
    // content when the sidebar opens or closes.
    canvasView.move(
        sf::Vector2f(
            (newCanvasWidth - previousCanvasWidth)
                / (2.0f * canvasZoom),
            0.0f
        )
    );

    updateViewLayout();
}

float PathlabApp::getUsableCanvasWidth() const {
    const float windowWidth = static_cast<float>(window.getSize().x);
    const float sidebarWidth = sidebarVisible ? PATHLAB_SIDE_PANEL_WIDTH : 0.0f;

    return std::max(1.0f, windowWidth - sidebarWidth);
}

void PathlabApp::updateViewLayout(){
    const sf::Vector2u windowSize = window.getSize();

    const float windowWidth =
        std::max(1.0f, static_cast<float>(windowSize.x));

    const float windowHeight =
        std::max(1.0f, static_cast<float>(windowSize.y));

    const float canvasWidth = getUsableCanvasWidth();

    const float canvasHeight = std::max(
        1.0f,
        windowHeight
        - PATHLAB_TOP_BAR_HEIGHT
        - PATHLAB_BOTTOM_BAR_HEIGHT
    );

    uiView = sf::View(
        sf::FloatRect(
            sf::Vector2f(0.0f, 0.0f),
            sf::Vector2f(windowWidth, windowHeight)
        )
    );

    canvasView.setSize(
        sf::Vector2f(
            canvasWidth / canvasZoom,
            canvasHeight / canvasZoom
        )
    );

    canvasView.setViewport(
        sf::FloatRect(
            sf::Vector2f(
                0.0f,
                PATHLAB_TOP_BAR_HEIGHT / windowHeight
            ),
            sf::Vector2f(
                canvasWidth / windowWidth,
                canvasHeight / windowHeight
            )
        )
    );

    worldBackdropDirty = true;
}

void PathlabApp::rebuildObstacleRenderCache(){
    worldBackdropDirty = true;

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
    worldBackdropDirty = true;

    visibilityGraphVertices.clear();
    visibilityNodeVertices.clear();
    exploredNodeVertices.clear();
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

    rebuildExploredNodeRenderCache();
    rebuildPlaybackHighlightCache();
}

void PathlabApp::rebuildExploredNodeRenderCache(){
    worldBackdropDirty = true;


    exploredNodeVertices.clear();


    const sf::Color exploredColor{
        59,
        130,
        246
    };


    const std::size_t visibleCount =
        std::min(
            playbackIndex,
            result.expandedNodeOrder.size()
        );


    for(std::size_t i = 0;
        i < visibleCount;
        ++i){

        const std::size_t nodeIndex =
            result.expandedNodeOrder[i];


        appendFilledCircle(
            exploredNodeVertices,
            graph.nodes[nodeIndex].position,
            5.0f,
            exploredColor
        );
    }
}

void PathlabApp::rebuildPlaybackHighlightCache(){
    worldBackdropDirty = true;

    currentExpandedNodeVertices.clear();
    goalReachedVertices.clear();

    if(!planningResultAvailable || !playbackActive){
        return;
    }

    const std::size_t expansionCount =
        result.expandedNodeOrder.size();

    if(playbackIndex >= expansionCount){
        if(graph.nodes.size() > 1){
            const Point& goalPosition = graph.nodes[1].position;

            appendFilledCircle(
                goalReachedVertices,
                goalPosition,
                20.0f,
                sf::Color(248, 113, 113, 45)
            );

            appendFilledCircle(
                goalReachedVertices,
                goalPosition,
                15.0f,
                sf::Color(248, 113, 113, 85)
            );
        }

        return;
    }

    if(playbackIndex == 0){
        return;
    }

    const std::size_t currentNodeIndex =
        result.expandedNodeOrder[playbackIndex - 1];

    const Point& currentPosition =
        graph.nodes[currentNodeIndex].position;

    appendFilledCircle(
        currentExpandedNodeVertices,
        currentPosition,
        15.0f,
        sf::Color(34, 211, 238, 65)
    );

    appendFilledCircle(
        currentExpandedNodeVertices,
        currentPosition,
        7.0f,
        sf::Color(34, 211, 238)
    );
}

void PathlabApp::updatePlayback(){

    if(!playbackPlaying){
        return;
    }


    if(
        playbackClock.getElapsedTime()
            .asSeconds()
        <
        playbackIntervalSeconds
    ){
        return;
    }


    playbackClock.restart();


    if(playbackIndex <
       result.expandedNodeOrder.size()){

        ++playbackIndex;

        rebuildExploredNodeRenderCache();
        rebuildPlaybackHighlightCache();
    }


    if(playbackIndex >=
       result.expandedNodeOrder.size()){

        playbackPlaying =
            false;
    }
}

void PathlabApp::updatePlaybackInterval(){

    switch(playbackSpeedIndex){

        case 0:
            playbackIntervalSeconds = 1.0f;
            break;

        case 1:
            playbackIntervalSeconds = 0.5f;
            break;

        case 2:
            playbackIntervalSeconds = 0.25f;
            break;

        case 3:
            playbackIntervalSeconds = 0.125f;
            break;

        case 4:
            playbackIntervalSeconds = 0.0625f;
            break;

        default:
            playbackSpeedIndex = 2;
            playbackIntervalSeconds = 0.25f;
            break;
    }
}
