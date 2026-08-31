#pragma once

#include <SFML/Graphics.hpp>

#include <optional>
#include <vector>

#include "../geometry/Point.hpp"
#include "../geometry/Polygon.hpp"
#include "../geometry/Segment.hpp"
#include "../graph/VisibilityGraph.hpp"
#include "../planners/Dijkstra.hpp"
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

class PathlabApp{
    public:
        PathlabApp();
        void run();
    
    private:
        // =====================================
        // Input
        // =====================================
        void processEvents();

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

        void drawObstacles();

        void drawCurrentObstacle();

        void drawStartAndGoal();

        void drawVisibilityGraph();

        void drawPath();

        PathlabUIData buildUIData() const;

        // =====================================
        // Window / resources
        // =====================================
        sf::RenderWindow window;

        sf::Font uiFont;

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
        Graph graph;

        DijkstraResult result;

        bool planningResultAvailable = false;

        bool showVisibilityGraph = true;

        double graphBuildTimeMs = 0.0;

        double searchTimeMs = 0.0;
};