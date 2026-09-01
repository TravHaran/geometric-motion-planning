#include "PathlabApp.hpp"

#include "../visualization/Renderer.hpp"
#include "../geometry/Segment.hpp"
#include "../graph/VisibilityGraph.hpp"
#include "../planners/AStar.hpp"
#include "../planners/BFS.hpp"
#include "../planners/Dijkstra.hpp"

#include <array>
#include <optional>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <iostream>

namespace{

constexpr float MIN_CANVAS_ZOOM = 0.25f;
constexpr float MAX_CANVAS_ZOOM = 4.0f;
constexpr float CANVAS_ZOOM_STEP = 1.15f;

struct PlannerOption{
    PlannerType type;
    const char* name;
};

constexpr std::array<PlannerOption, 3> PLANNER_OPTIONS = {{
    {PlannerType::BFS, "BFS"},
    {PlannerType::Dijkstra, "Dijkstra"},
    {PlannerType::AStar, "A*"}
}};

std::size_t getPlannerOptionIndex(PlannerType planner){
    for(std::size_t i = 0; i < PLANNER_OPTIONS.size(); ++i){
        if(PLANNER_OPTIONS[i].type == planner){
            return i;
        }
    }

    return 0;
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

    const PathlabUIInteraction uiInteraction =
        handlePathlabUIClick(
            event.position,
            window.getSize(),
            uiData
        );

    switch(uiInteraction.action){

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

        case PathlabUIAction::SelectAlgorithm:

            algorithmDropdownOpen =
                false;

            if(uiInteraction.algorithmIndex >= PLANNER_OPTIONS.size()){
                return;
            }

            if(selectedPlanner !=
            PLANNER_OPTIONS[uiInteraction.algorithmIndex].type){

                selectedPlanner =
                    PLANNER_OPTIONS[uiInteraction.algorithmIndex].type;

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

    switch(selectedPlanner){
        case PlannerType::BFS:{
            const BFSResult plannerResult = bfs(graph, 0, 1);

            result = {
                plannerResult.distance,
                plannerResult.path,
                plannerResult.expandedNodes,
                plannerResult.expandedNodeOrder
            };

            break;
        }

        case PlannerType::Dijkstra:{
            const DijkstraResult plannerResult = dijkstra(graph, 0, 1);

            result = {
                plannerResult.distance,
                plannerResult.path,
                plannerResult.expandedNodes,
                plannerResult.expandedNodeOrder
            };

            break;
        }

        case PlannerType::AStar:{
            const AStarResult plannerResult = aStar(graph, 0, 1);

            result = {
                plannerResult.distance,
                plannerResult.path,
                plannerResult.expandedNodes,
                plannerResult.expandedNodeOrder
            };

            break;
        }
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

    data.algorithmOptions.reserve(PLANNER_OPTIONS.size());

    for(const PlannerOption& option : PLANNER_OPTIONS){
        data.algorithmOptions.push_back(option.name);
    }

    data.selectedAlgorithmIndex =
        getPlannerOptionIndex(selectedPlanner);

    data.algorithmDropdownOpen = algorithmDropdownOpen;
    data.helpOverlayOpen = helpOverlayOpen;
    data.sidebarVisible = sidebarVisible;
    data.canRunPlanner = canRunPlanner();

    if(!start.has_value() && !goal.has_value()){
        data.plannerStatus =
            PathlabPlannerStatus::SetStartAndGoal;
    }
    else if(!start.has_value()){
        data.plannerStatus =
            PathlabPlannerStatus::SetStart;
    }
    else if(!goal.has_value()){
        data.plannerStatus =
            PathlabPlannerStatus::SetGoal;
    }
    else if(!planningResultAvailable){
        data.plannerStatus =
            PathlabPlannerStatus::Ready;
    }
    else if(result.path.empty()){
        data.plannerStatus =
            PathlabPlannerStatus::NoPath;
    }
    else{
        data.plannerStatus =
            PathlabPlannerStatus::PathFound;
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
