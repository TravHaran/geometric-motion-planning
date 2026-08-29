#pragma once

#include <SFML/Graphics.hpp>
#include <cstddef>
#include <string>

constexpr float DEMO_PANEL_WIDTH = 260.0f;

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
};

bool loadDemoPanelFont(
    sf::Font& font
);

void drawDemoPanel(
    sf::RenderWindow& window,
    const sf::Font& font,
    const DemoPanelData& data
);