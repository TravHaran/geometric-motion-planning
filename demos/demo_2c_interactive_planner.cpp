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

#include "../src/planners/Dijkstra.hpp"

#include "../src/visualization/Renderer.hpp"
#include "../src/visualization/DemoPanel.hpp"


enum class InputMode{
    Obstacle,
    Start,
    Goal
};


int main()
{
    sf::RenderWindow window(
        sf::VideoMode({1000, 700}),
        "Demo 2C - Interactive Motion Planner"
    );


    window.setMinimumSize(
        sf::Vector2u{
            700,
            650
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

    std::vector<Point> currentObstacleVertices;

    std::optional<Point> start;
    std::optional<Point> goal;


    InputMode inputMode =
        InputMode::Obstacle;


    // =====================================
    // Planning state
    // =====================================

    Graph graph;

    DijkstraResult result;

    bool planningResultAvailable = false;

    bool showVisibilityGraph = true;

    double graphBuildTimeMs = 0.0;
    double searchTimeMs = 0.0;


    /*
     * Clears planner output whenever the
     * environment changes.
     */
    auto invalidatePlanningResult = [&](){

        graph.nodes.clear();
        graph.edges.clear();

        result.path.clear();

        graphBuildTimeMs = 0.0;
        searchTimeMs = 0.0;

        planningResultAvailable = false;
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

            if(event->is<sf::Event::Closed>()){
                window.close();
            }


            // ---------------------------------
            // Window resize
            // ---------------------------------

            if(const auto* resized =
                event->getIf<sf::Event::Resized>()){

                sf::FloatRect visibleArea(
                    {0.0f, 0.0f},
                    sf::Vector2f(resized->size)
                );

                window.setView(
                    sf::View(visibleArea)
                );
            }


            // =================================
            // Mouse input
            // =================================

            if(const auto* mousePressed =
                event->getIf<
                    sf::Event::MouseButtonPressed
                >()){

                if(mousePressed->button ==
                   sf::Mouse::Button::Left){

                    const float canvasWidth =
                        static_cast<float>(
                            window.getSize().x
                        )
                        - DEMO_PANEL_WIDTH;


                    if(mousePressed->position.x <
                       static_cast<int>(
                           canvasWidth
                       )){

                        Point clickedPoint{
                            static_cast<double>(
                                mousePressed->position.x
                            ),
                            static_cast<double>(
                                mousePressed->position.y
                            )
                        };


                        // -------------------------
                        // Obstacle mode
                        // -------------------------

                        if(inputMode ==
                           InputMode::Obstacle){

                            currentObstacleVertices.push_back(
                                clickedPoint
                            );
                        }


                        // -------------------------
                        // Start placement
                        // -------------------------

                        else if(inputMode ==
                                InputMode::Start){

                            start =
                                clickedPoint;

                            invalidatePlanningResult();

                            inputMode =
                                InputMode::Obstacle;
                        }


                        // -------------------------
                        // Goal placement
                        // -------------------------

                        else if(inputMode ==
                                InputMode::Goal){

                            goal =
                                clickedPoint;

                            invalidatePlanningResult();

                            inputMode =
                                InputMode::Obstacle;
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

                    if(currentObstacleVertices.size()
                       >= 3){

                        Polygon obstacle{
                            currentObstacleVertices
                        };

                        obstacles.push_back(
                            obstacle
                        );

                        currentObstacleVertices.clear();

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
                }


                // -----------------------------
                // Set goal mode
                // -----------------------------

                else if(keyPressed->code ==
                        sf::Keyboard::Key::G){

                    inputMode =
                        InputMode::Goal;
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
                        sf::Keyboard::Key::Escape){

                    if(inputMode !=
                       InputMode::Obstacle){

                        inputMode =
                            InputMode::Obstacle;
                    }
                    else{

                        currentObstacleVertices.clear();
                    }
                }


                // -----------------------------
                // Run planner
                // -----------------------------

                else if(keyPressed->code ==
                        sf::Keyboard::Key::Space){

                    if(start.has_value() &&
                       goal.has_value()){


                        // -------------------------
                        // Build visibility graph
                        // -------------------------

                        auto graphBuildStart =
                            std::chrono::
                            steady_clock::now();


                        graph =
                            buildVisibilityGraph(
                                *start,
                                *goal,
                                obstacles
                            );


                        auto graphBuildEnd =
                            std::chrono::
                            steady_clock::now();


                        graphBuildTimeMs =
                            std::chrono::duration<
                                double,
                                std::milli
                            >(
                                graphBuildEnd
                                - graphBuildStart
                            ).count();


                        // -------------------------
                        // Run Dijkstra
                        // -------------------------

                        auto searchStart =
                            std::chrono::
                            steady_clock::now();


                        result =
                            dijkstra(
                                graph,
                                0,
                                1
                            );


                        auto searchEnd =
                            std::chrono::
                            steady_clock::now();


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

                    currentObstacleVertices.clear();

                    start.reset();
                    goal.reset();

                    invalidatePlanningResult();

                    inputMode =
                        InputMode::Obstacle;
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
                i + 1 < result.path.size();
                ++i){

                std::size_t fromNode =
                    result.path[i];

                std::size_t toNode =
                    result.path[i + 1];


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
                currentObstacleVertices.size();
            ++i){

            Segment edge{
                currentObstacleVertices[i],
                currentObstacleVertices[i + 1]
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
        else if(result.path.empty()){

            plannerText =
                "No Path";
        }
        else{

            plannerText =
                "Path Found";
        }


        // ---------------------------------
        // Path metrics
        // ---------------------------------

        double pathLength = 0.0;


        if(planningResultAvailable &&
           std::isfinite(
               result.distance
           )){

            pathLength =
                result.distance;
        }


        std::size_t pathSegments = 0;


        if(!result.path.empty()){

            pathSegments =
                result.path.size() - 1;
        }


        const double totalTimeMs =
            graphBuildTimeMs
            + searchTimeMs;


        DemoPanelData panelData{
            modeText,
            graphText,
            plannerText,

            "Dijkstra",

            pathLength,
            pathSegments,

            graph.nodes.size(),
            graph.edges.size(),
            obstacles.size(),

            graphBuildTimeMs,
            searchTimeMs,
            totalTimeMs
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