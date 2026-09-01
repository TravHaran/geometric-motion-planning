#pragma once

#include <SFML/Graphics.hpp>

#include <string>

constexpr float PATHLAB_TOP_BAR_HEIGHT = 64.0f;
constexpr float PATHLAB_BOTTOM_BAR_HEIGHT = 48.0f;
constexpr float PATHLAB_SIDE_PANEL_WIDTH = 300.0f;

enum class PathlabUIAction{
    None,
    RunPlanner,
    ToggleObstacles,
    ToggleVisibilityGraph,
    ToggleFinalPath,
    ToggleExploredNodes,
    ResetPlayback,
    StepPlayback,
    TogglePlayback,
    CyclePlaybackSpeed,
    ToggleAlgorithmDropdown,
    SelectDijkstra,
    SelectAStar,
    CloseAlgorithmDropdown
};

struct PathlabUIData{
    std::string algorithm;

    bool algorithmDropdownOpen = false;

    std::string plannerStatus;

    std::size_t obstacleCount = 0;
    std::size_t graphNodeCount = 0;
    std::size_t graphEdgeCount = 0;

    double pathLength = 0.0;
    std::size_t pathSegments = 0;
    std::size_t pathNodes = 0;
    std::size_t nodesExpanded = 0;

    double graphBuildTimeMs = 0.0;
    double searchTimeMs = 0.0;
    double totalTimeMs = 0.0;

    bool showObstacles = true;
    bool showVisibilityGraph = true;
    bool showFinalPath = true;

    bool showExploredNodes = true;
    bool hasSearchTrace = false;

    std::size_t playbackIndex = 0;

    std::size_t playbackTotal = 0;

    bool playbackPlaying = false;

    std::string playbackSpeed;
};

bool loadPathlabFont(sf::Font& font);

void drawPathlabUI(
    sf::RenderWindow& window, 
    const sf::Font& font, 
    const PathlabUIData& data
);

PathlabUIAction handlePathlabUIClick(
    const sf::Vector2i& position,
    const sf::Vector2u& windowSize,
    bool algorithmDropdownOpen,
    bool hasSearchTrace
);