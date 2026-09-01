#pragma once

#include <SFML/Graphics.hpp>

#include <optional>
#include <vector>

#include "../geometry/Point.hpp"
#include "../geometry/Polygon.hpp"
#include "../geometry/Segment.hpp"
#include "../graph/VisibilityGraph.hpp"
#include "../planners/Dijkstra.hpp"
#include "../planners/AStar.hpp"
#include "../ui/PathlabUI.hpp"

/**
 * This represents the actual application
 * it has three responsibilities
 * run()
 *      application loop
 * processEvents()
 *      keyboard/window/mouse events
 * render()
 *      draw a frame
 */

enum class InputMode{
    Obstacle,
    Start, 
    Goal
};

enum class PlannerType{
    Dijkstra,
    AStar
};

class PathlabApp{
    public:
        PathlabApp();
        void run();
    
    private:
        // =====================================
        // Input
        // =====================================
        void processEvents();

        void processEvent(const sf::Event& event);

        void handleMousePressed(
            const sf::Event::MouseButtonPressed& event
        );

        void handleKeyPressed(
            const sf::Event::KeyPressed& event
        );

        bool isInsideCanvas(const sf::Vector2i& position) const;

        // =====================================
        // Scene 
        // =====================================
        void resetScene();

        // =====================================
        // Planning
        // =====================================
        void runPlanner();

        void invalidatePlanningResult();

        // =====================================
        // Rendering
        // =====================================
        void render();

        PathlabUIData buildUIData() const;

        void drawCanvasBackground();

        void drawObstacles();

        void drawCurrentObstacle();

        void drawStartAndGoal();

        void drawVisibilityGraph();

        void drawPath();

        void rebuildCanvasRenderCache();

        void rebuildObstacleRenderCache();

        void rebuildPlanningRenderCache();

        // =====================================
        // Window / resources
        // =====================================
        sf::RenderWindow window;

        sf::Font uiFont;

        sf::VertexArray canvasGridVertices{sf::PrimitiveType::Lines};

        sf::VertexArray obstacleFillVertices{sf::PrimitiveType::Triangles};

        sf::VertexArray obstacleBorderVertices{sf::PrimitiveType::Lines};

        sf::VertexArray obstacleMarkerVertices{sf::PrimitiveType::Triangles};

        sf::VertexArray visibilityGraphVertices{sf::PrimitiveType::Lines};

        sf::VertexArray visibilityNodeVertices{sf::PrimitiveType::Triangles};

        sf::VertexArray pathVertices{sf::PrimitiveType::Triangles};

        sf::VertexArray pathMarkerVertices{sf::PrimitiveType::Triangles};

        // =====================================
        // Scene state
        // =====================================

        std::vector<Polygon> obstacles;
        std::vector<Point> currentObstacleVertices;
        std::optional<Point> start;
        std::optional<Point> goal;

        InputMode inputMode = InputMode::Obstacle;

        // =====================================
        // Planning state
        // =====================================
        struct PlanningResult{
            double distance = 0.0;
            std::vector<std::size_t> path;
            std::size_t expandedNodes = 0;
        };

        Graph graph;

        PlanningResult result;

        PlannerType selectedPlanner = PlannerType::Dijkstra;

        bool algorithmDropdownOpen = false;

        bool planningResultAvailable = false;

        bool showObstacles = true;

        bool showVisibilityGraph = true;

        bool showFinalPath = true;

        double graphBuildTimeMs = 0.0;

        double searchTimeMs = 0.0;
};
