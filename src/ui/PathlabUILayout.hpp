#pragma once

#include <SFML/Graphics.hpp>

#include <cstddef>

namespace pathlab_ui_detail{

bool useCompactSidebarLayout(const sf::Vector2u& windowSize);

float getUsableCanvasWidth(
    const sf::Vector2u& windowSize,
    bool sidebarVisible
);

sf::FloatRect getRunPlannerButtonBounds(const sf::Vector2u& windowSize);

sf::FloatRect getResetCameraButtonBounds(
    const sf::Vector2u& windowSize,
    bool sidebarVisible
);

sf::FloatRect getLoadDemoSceneButtonBounds(
    const sf::Vector2u& windowSize,
    bool sidebarVisible
);

sf::FloatRect getPlaybackDockBounds(
    const sf::Vector2u& windowSize,
    bool sidebarVisible
);

sf::FloatRect getPlaybackResetBounds(
    const sf::Vector2u& windowSize,
    bool sidebarVisible
);

sf::FloatRect getPlaybackStepBounds(
    const sf::Vector2u& windowSize,
    bool sidebarVisible
);

sf::FloatRect getPlaybackToggleBounds(
    const sf::Vector2u& windowSize,
    bool sidebarVisible
);

sf::FloatRect getPlaybackSpeedBounds(
    const sf::Vector2u& windowSize,
    bool sidebarVisible
);

sf::FloatRect getVisualizationRowBounds(
    const sf::Vector2u& windowSize,
    std::size_t rowIndex
);

sf::FloatRect getAlgorithmSelectorBounds(const sf::Vector2u& windowSize);

sf::FloatRect getAlgorithmOptionBounds(
    const sf::Vector2u& windowSize,
    std::size_t optionIndex
);

bool containsPoint(
    const sf::FloatRect& bounds,
    const sf::Vector2i& position
);

sf::FloatRect getHelpPanelBounds(const sf::Vector2u& windowSize);

sf::FloatRect getHelpButtonBounds(const sf::Vector2u& windowSize);

sf::FloatRect getSidebarToggleButtonBounds(const sf::Vector2u& windowSize);

sf::FloatRect getHelpCloseButtonBounds(const sf::Vector2u& windowSize);

}
