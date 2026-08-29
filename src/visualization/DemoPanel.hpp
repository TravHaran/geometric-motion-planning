#pragma once

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <string>


constexpr float DEMO_PANEL_WIDTH =
    260.0f;


/*
 * Describes what happened when the user
 * clicked the algorithm selector.
 *
 * This remains a UI concept. The panel
 * does not know anything about Dijkstra
 * or A* directly.
 */
enum class AlgorithmSelectorAction{
    None,
    Toggle,
    SelectDijkstra,
    SelectAStar
};


/*
 * Contains only information that the UI needs
 * in order to display the current demo state.
 *
 * The panel does not know anything about the
 * underlying planner or graph implementation.
 */
struct DemoPanelData{
    std::string mode;
    std::string graphStatus;
    std::string plannerStatus;

    std::string algorithm;

    double pathLength;
    std::size_t pathSegments;

    std::size_t graphNodes;
    std::size_t graphEdges;
    std::size_t obstacleCount;

    double graphBuildTimeMs;
    double searchTimeMs;
    double totalTimeMs;

    // Optional newer-demo information.
    std::size_t expandedNodes = 0;

    bool showAlgorithmSelector = false;
    bool algorithmDropdownOpen = false;
};


bool loadDemoPanelFont(
    sf::Font& font
);


void drawDemoPanel(
    sf::RenderWindow& window,
    const sf::Font& font,
    const DemoPanelData& data
);


/*
 * Determines whether a mouse click interacted
 * with the algorithm dropdown.
 */
AlgorithmSelectorAction getAlgorithmSelectorAction(
    const sf::Vector2i& mousePosition,
    const sf::Vector2u& windowSize,
    bool dropdownOpen
);