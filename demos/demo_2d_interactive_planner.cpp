#include <SFML/Graphics.hpp>

#include <chrono>
#include <cmath>
#include <optional>
#include <string>
#include <vector>

#include "../src/geometry/Point.hpp"
#include "../src/geometry/Polygon.hpp"
#include "../src/geometry/Segment.hpp"

#include "../src/graph/VisibilityGraph.hpp"

#include "../src/planners/AStar.hpp"
#include "../src/planners/Dijkstra.hpp"

#include "../src/visualization/Renderer.hpp"
#include "../src/visualization/DemoPanel.hpp"


enum class InputMode{
    Obstacle,
    Start,
    Goal
};


enum class PlanningAlgorithm{
    Dijkstra,
    AStar
};


int main()
{
    sf::RenderWindow window(
        sf::VideoMode({1000, 760}),
        "Demo 2D - Interactive Planner Comparison"
    );


    window.setMinimumSize(
        sf::Vector2u{
            700,
            720
        }
    );


    // =====================================
    // UI resources
    // =====================================

    sf::Font panelFont;


    if(!loadDemoPanelFont(
        panelFont
    )){
        return 1;
    }


    // =====================================
    // Environment state
    // =====================================

    std::vector<Polygon> obstacles;

    std::vector<Point>
        currentObstacleVertices;

    std::optional<Point> start;
    std::optional<Point> goal;


    InputMode inputMode =
        InputMode::Obstacle;


    // =====================================
    // Algorithm selection
    // =====================================

    PlanningAlgorithm selectedAlgorithm =
        PlanningAlgorithm::Dijkstra;


    bool algorithmDropdownOpen =
        false;


    // =====================================
    // Planning state
    // =====================================

    Graph graph;


    /*
     * The visualizer stores planner-independent
     * output rather than storing either a
     * DijkstraResult or AStarResult.
     */
    std::vector<std::size_t>
        plannedPath;


    double pathDistance =
        0.0;


    std::size_t expandedNodes =
        0;


    bool planningResultAvailable =
        false;


    bool showVisibilityGraph =
        true;


    double graphBuildTimeMs =
        0.0;


    double searchTimeMs =
        0.0;


    /*
     * Clears planner output whenever either
     * the environment or selected algorithm
     * changes.
     *
     * Obstacles, start, and goal are preserved.
     */
    auto invalidatePlanningResult = [&](){

        graph.nodes.clear();
        graph.edges.clear();

        plannedPath.clear();

        pathDistance =
            0.0;

        expandedNodes =
            0;

        graphBuildTimeMs =
            0.0;

        searchTimeMs =
            0.0;

        planningResultAvailable =
            false;
    };


    // =====================================
    // Main application loop
    // =====================================

    while(window.isOpen()){


        while(const std::optional event =
            window.pollEvent()){


            // ---------------------------------
            // Window close
            // ---------------------------------

            if(event->is<
                sf::Event::Closed
            >()){

                window.close();
            }


            // ---------------------------------
            // Window resize
            // ---------------------------------

            if(const auto* resized =
                event->getIf<
                    sf::Event::Resized
                >()){

                sf::FloatRect visibleArea(
                    {0.0f, 0.0f},
                    sf::Vector2f(
                        resized->size
                    )
                );


                window.setView(
                    sf::View(
                        visibleArea
                    )
                );
            }


            // =================================
            // Mouse input
            // =================================

            if(const auto* mousePressed =
                event->getIf<
                    sf::Event::
                        MouseButtonPressed
                >()){


                if(mousePressed->button ==
                   sf::Mouse::Button::Left){


                    const float canvasWidth =
                        static_cast<float>(
                            window.getSize().x
                        )
                        - DEMO_PANEL_WIDTH;


                    // =========================
                    // Canvas click
                    // =========================

                    if(mousePressed->position.x <
                       static_cast<int>(
                           canvasWidth
                       )){


                        algorithmDropdownOpen =
                            false;


                        Point clickedPoint{
                            static_cast<double>(
                                mousePressed->
                                    position.x
                            ),
                            static_cast<double>(
                                mousePressed->
                                    position.y
                            )
                        };


                        // -------------------------
                        // Obstacle mode
                        // -------------------------

                        if(inputMode ==
                           InputMode::Obstacle){

                            currentObstacleVertices.
                                push_back(
                                    clickedPoint
                                );
                        }


                        // -------------------------
                        // Start placement
                        // -------------------------

                        else if(
                            inputMode ==
                            InputMode::Start
                        ){

                            start =
                                clickedPoint;


                            invalidatePlanningResult();


                            inputMode =
                                InputMode::Obstacle;
                        }


                        // -------------------------
                        // Goal placement
                        // -------------------------

                        else if(
                            inputMode ==
                            InputMode::Goal
                        ){

                            goal =
                                clickedPoint;


                            invalidatePlanningResult();


                            inputMode =
                                InputMode::Obstacle;
                        }
                    }


                    // =========================
                    // Panel click
                    // =========================

                    else{


                        const AlgorithmSelectorAction
                            selectorAction =
                                getAlgorithmSelectorAction(
                                    mousePressed->
                                        position,
                                    window.getSize(),
                                    algorithmDropdownOpen
                                );


                        // -------------------------
                        // Open / close dropdown
                        // -------------------------

                        if(selectorAction ==
                           AlgorithmSelectorAction::
                               Toggle){

                            algorithmDropdownOpen =
                                !algorithmDropdownOpen;
                        }


                        // -------------------------
                        // Select Dijkstra
                        // -------------------------

                        else if(
                            selectorAction ==
                            AlgorithmSelectorAction::
                                SelectDijkstra
                        ){

                            algorithmDropdownOpen =
                                false;


                            if(selectedAlgorithm !=
                               PlanningAlgorithm::
                                   Dijkstra){

                                selectedAlgorithm =
                                    PlanningAlgorithm::
                                        Dijkstra;


                                invalidatePlanningResult();
                            }
                        }


                        // -------------------------
                        // Select A*
                        // -------------------------

                        else if(
                            selectorAction ==
                            AlgorithmSelectorAction::
                                SelectAStar
                        ){

                            algorithmDropdownOpen =
                                false;


                            if(selectedAlgorithm !=
                               PlanningAlgorithm::
                                   AStar){

                                selectedAlgorithm =
                                    PlanningAlgorithm::
                                        AStar;


                                invalidatePlanningResult();
                            }
                        }


                        else{

                            algorithmDropdownOpen =
                                false;
                        }
                    }
                }
            }


            // =================================
            // Keyboard input
            // =================================

            if(const auto* keyPressed =
                event->getIf<
                    sf::Event::KeyPressed
                >()){


                // -----------------------------
                // Finalize obstacle
                // -----------------------------

                if(keyPressed->code ==
                   sf::Keyboard::Key::Enter){


                    if(currentObstacleVertices.
                       size() >= 3){


                        Polygon obstacle{
                            currentObstacleVertices
                        };


                        obstacles.push_back(
                            obstacle
                        );


                        currentObstacleVertices.
                            clear();


                        invalidatePlanningResult();
                    }
                }


                // -----------------------------
                // Set start mode
                // -----------------------------

                else if(keyPressed->code ==
                        sf::Keyboard::Key::S){

                    inputMode =
                        InputMode::Start;


                    algorithmDropdownOpen =
                        false;
                }


                // -----------------------------
                // Set goal mode
                // -----------------------------

                else if(keyPressed->code ==
                        sf::Keyboard::Key::G){

                    inputMode =
                        InputMode::Goal;


                    algorithmDropdownOpen =
                        false;
                }


                // -----------------------------
                // Toggle visibility graph
                // -----------------------------

                else if(keyPressed->code ==
                        sf::Keyboard::Key::V){

                    showVisibilityGraph =
                        !showVisibilityGraph;
                }


                // -----------------------------
                // Cancel
                // -----------------------------

                else if(keyPressed->code ==
                        sf::Keyboard::Key::
                            Escape){


                    if(algorithmDropdownOpen){

                        algorithmDropdownOpen =
                            false;
                    }


                    else if(inputMode !=
                            InputMode::Obstacle){

                        inputMode =
                            InputMode::Obstacle;
                    }


                    else{

                        currentObstacleVertices.
                            clear();
                    }
                }


                // -----------------------------
                // Run planner
                // -----------------------------

                else if(keyPressed->code ==
                        sf::Keyboard::Key::
                            Space){


                    algorithmDropdownOpen =
                        false;


                    if(start.has_value() &&
                       goal.has_value()){


                        // =========================
                        // Build visibility graph
                        // =========================

                        const auto graphBuildStart =
                            std::chrono::
                                steady_clock::
                                now();


                        graph =
                            buildVisibilityGraph(
                                *start,
                                *goal,
                                obstacles
                            );


                        const auto graphBuildEnd =
                            std::chrono::
                                steady_clock::
                                now();


                        graphBuildTimeMs =
                            std::chrono::duration<
                                double,
                                std::milli
                            >(
                                graphBuildEnd
                                - graphBuildStart
                            ).count();


                        // =========================
                        // Run selected algorithm
                        // =========================

                        const auto searchStart =
                            std::chrono::
                                steady_clock::
                                now();


                        if(selectedAlgorithm ==
                           PlanningAlgorithm::
                               Dijkstra){


                            const DijkstraResult
                                result =
                                    dijkstra(
                                        graph,
                                        0,
                                        1
                                    );


                            plannedPath =
                                result.path;


                            pathDistance =
                                result.distance;


                            expandedNodes =
                                result.expandedNodes;
                        }


                        else{


                            const AStarResult
                                result =
                                    aStar(
                                        graph,
                                        0,
                                        1
                                    );


                            plannedPath =
                                result.path;


                            pathDistance =
                                result.distance;


                            expandedNodes =
                                result.expandedNodes;
                        }


                        const auto searchEnd =
                            std::chrono::
                                steady_clock::
                                now();


                        searchTimeMs =
                            std::chrono::duration<
                                double,
                                std::milli
                            >(
                                searchEnd
                                - searchStart
                            ).count();


                        planningResultAvailable =
                            true;
                    }
                }


                // -----------------------------
                // Reset
                // -----------------------------

                else if(keyPressed->code ==
                        sf::Keyboard::Key::R){


                    obstacles.clear();


                    currentObstacleVertices.
                        clear();


                    start.reset();
                    goal.reset();


                    invalidatePlanningResult();


                    inputMode =
                        InputMode::Obstacle;


                    algorithmDropdownOpen =
                        false;
                }
            }
        }


        // =====================================
        // Rendering
        // =====================================

        window.clear();


        // ---------------------------------
        // Draw visibility graph
        // ---------------------------------

        if(planningResultAvailable &&
           showVisibilityGraph){


            drawGraph(
                window,
                graph,
                sf::Color(
                    120,
                    120,
                    120
                )
            );
        }


        // ---------------------------------
        // Draw finalized obstacles
        // ---------------------------------

        for(const Polygon& obstacle :
            obstacles){


            const std::size_t n =
                obstacle.vertices.size();


            for(std::size_t i = 0;
                i < n;
                ++i){


                Segment edge{
                    obstacle.vertices[i],
                    obstacle.vertices[
                        (i + 1) % n
                    ]
                };


                drawSegment(
                    window,
                    edge,
                    sf::Color::Red
                );
            }
        }


        // ---------------------------------
        // Draw shortest path
        // ---------------------------------

        if(planningResultAvailable){


            for(std::size_t i = 0;
                i + 1 <
                    plannedPath.size();
                ++i){


                const std::size_t fromNode =
                    plannedPath[i];


                const std::size_t toNode =
                    plannedPath[
                        i + 1
                    ];


                const Point& a =
                    graph.nodes[
                        fromNode
                    ].position;


                const Point& b =
                    graph.nodes[
                        toNode
                    ].position;


                Segment pathSegment{
                    a,
                    b
                };


                drawSegment(
                    window,
                    pathSegment,
                    sf::Color::Green
                );
            }
        }


        // ---------------------------------
        // Draw unfinished obstacle
        // ---------------------------------

        for(std::size_t i = 0;
            i + 1 <
                currentObstacleVertices.
                    size();
            ++i){


            Segment edge{
                currentObstacleVertices[i],
                currentObstacleVertices[
                    i + 1
                ]
            };


            drawSegment(
                window,
                edge,
                sf::Color::White
            );
        }


        for(const Point& vertex :
            currentObstacleVertices){


            drawPoint(
                window,
                vertex,
                sf::Color::White
            );
        }


        // ---------------------------------
        // Draw graph nodes
        // ---------------------------------

        if(planningResultAvailable &&
           showVisibilityGraph){


            for(const GraphNode& node :
                graph.nodes){


                drawPoint(
                    window,
                    node.position,
                    sf::Color::White
                );
            }
        }


        // ---------------------------------
        // Draw start
        // ---------------------------------

        if(start.has_value()){


            drawPoint(
                window,
                *start,
                sf::Color::Green
            );
        }


        // ---------------------------------
        // Draw goal
        // ---------------------------------

        if(goal.has_value()){


            drawPoint(
                window,
                *goal,
                sf::Color::Red
            );
        }


        // =====================================
        // Prepare UI state
        // =====================================

        std::string modeText;


        if(inputMode ==
           InputMode::Obstacle){

            modeText =
                "Obstacle";
        }


        else if(inputMode ==
                InputMode::Start){

            modeText =
                "Set Start";
        }


        else{

            modeText =
                "Set Goal";
        }


        std::string graphText;


        if(showVisibilityGraph){

            graphText =
                "Visible";
        }


        else{

            graphText =
                "Hidden";
        }


        std::string plannerText;


        if(!planningResultAvailable){

            plannerText =
                "Not Run";
        }


        else if(plannedPath.empty()){

            plannerText =
                "No Path";
        }


        else{

            plannerText =
                "Path Found";
        }


        // ---------------------------------
        // Algorithm name
        // ---------------------------------

        std::string algorithmText;


        if(selectedAlgorithm ==
           PlanningAlgorithm::Dijkstra){

            algorithmText =
                "Dijkstra";
        }


        else{

            algorithmText =
                "A*";
        }


        // ---------------------------------
        // Path metrics
        // ---------------------------------

        double pathLength =
            0.0;


        if(planningResultAvailable &&
           std::isfinite(
               pathDistance
           )){

            pathLength =
                pathDistance;
        }


        std::size_t pathSegments =
            0;


        if(!plannedPath.empty()){

            pathSegments =
                plannedPath.size()
                - 1;
        }


        const double totalTimeMs =
            graphBuildTimeMs
            + searchTimeMs;


        DemoPanelData panelData{
            modeText,
            graphText,
            plannerText,

            algorithmText,

            pathLength,
            pathSegments,

            graph.nodes.size(),
            graph.edges.size(),
            obstacles.size(),

            graphBuildTimeMs,
            searchTimeMs,
            totalTimeMs,

            expandedNodes,

            true,
            algorithmDropdownOpen
        };


        // =====================================
        // Draw UI panel last
        // =====================================

        drawDemoPanel(
            window,
            panelFont,
            panelData
        );


        window.display();
    }


    return 0;
}