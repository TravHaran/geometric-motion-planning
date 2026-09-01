#include "PathlabApp.hpp"

#include "../geometry/Triangulation.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace{

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
