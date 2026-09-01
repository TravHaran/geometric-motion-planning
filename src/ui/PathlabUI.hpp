#pragma once

#include <SFML/Graphics.hpp>

#include <string>

constexpr float PATHLAB_TOP_BAR_HEIGHT = 64.0f;
constexpr float PATHLAB_BOTTOM_BAR_HEIGHT = 48.0f;
constexpr float PATHLAB_SIDE_PANEL_WIDTH = 300.0f;

enum class PathlabUIAction{
    None,
    OpenHelpOverlay,
    ToggleSidebar,
    LoadDemoScene,
    ResetCamera,
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

    bool helpOverlayOpen = false;

    bool sidebarVisible = true;

    std::string plannerStatus;

    bool canRunPlanner = false;

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

    bool hasObstacles = false;
    bool hasVisibilityGraph = false;
    bool hasFinalPath = false;

    bool showExploredNodes = true;
    bool hasSearchTrace = false;

    std::size_t playbackIndex = 0;

    std::size_t playbackTotal = 0;

    bool playbackPlaying = false;

    std::string playbackSpeed;
};

struct PathlabGlassBackdrop{
    const sf::Texture* texture = nullptr;
    sf::Vector2u sourceWindowSize;
};

bool loadPathlabFont(sf::Font& font);

void drawPathlabUI(
    sf::RenderWindow& window, 
    const sf::Font& font, 
    const PathlabUIData& data,
    const PathlabGlassBackdrop& glassBackdrop
);

PathlabUIAction handlePathlabUIClick(
    const sf::Vector2i& position,
    const sf::Vector2u& windowSize,
    const PathlabUIData& data
);

bool isPathlabUIOverlayAt(
    const sf::Vector2i& position,
    const sf::Vector2u& windowSize,
    bool hasSearchTrace,
    bool sidebarVisible
);

bool isPathlabHelpCloseButtonAt(
    const sf::Vector2i& position,
    const sf::Vector2u& windowSize
);
