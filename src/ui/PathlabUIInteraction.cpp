#include "PathlabUI.hpp"

#include "PathlabUILayout.hpp"
#include "PathlabSidebar.hpp"

using namespace pathlab_ui_detail;

PathlabUIInteraction handlePathlabUIClick(
    const sf::Vector2i& position,
    const sf::Vector2u& windowSize,
    const PathlabUIData& data
){
    if(containsPoint(getHelpButtonBounds(windowSize),position))
        return PathlabUIAction::OpenHelpOverlay;
    if(containsPoint(getSidebarToggleButtonBounds(windowSize),position))
        return PathlabUIAction::ToggleSidebar;
    if(containsPoint(getLoadDemoSceneButtonBounds(windowSize,data.sidebarVisible),position))
        return PathlabUIAction::LoadDemoScene;
    if(containsPoint(getResetCameraButtonBounds(windowSize,data.sidebarVisible),position))
        return PathlabUIAction::ResetCamera;

    // All scrollable sidebar controls share their render/layout hitboxes.
    const auto sidebar=pathlab_sidebar::hitTest(position,windowSize,data);
    if(sidebar.action!=PathlabUIAction::None)return sidebar;

    // Playback dock is in the canvas, independent of sidebar scrolling.
    if(data.hasSearchTrace){
        if(containsPoint(getPlaybackResetBounds(windowSize,data.sidebarVisible),position))
            return PathlabUIAction::ResetPlayback;
        if(containsPoint(getPlaybackToggleBounds(windowSize,data.sidebarVisible),position))
            return PathlabUIAction::TogglePlayback;
        if(containsPoint(getPlaybackSpeedBounds(windowSize,data.sidebarVisible),position))
            return PathlabUIAction::CyclePlaybackSpeed;
        if(containsPoint(getPlaybackStepBounds(windowSize,data.sidebarVisible),position))
            return PathlabUIAction::StepPlayback;
    }
    if(data.algorithmDropdownOpen)return PathlabUIAction::CloseAlgorithmDropdown;
    return PathlabUIAction::None;
}

bool isPathlabUIOverlayAt(
    const sf::Vector2i& position,
    const sf::Vector2u& windowSize,
    bool hasSearchTrace,
    bool sidebarVisible
){
    return
        hasSearchTrace
        &&
        containsPoint(
            getPlaybackDockBounds(windowSize, sidebarVisible),
            position
        );
}

bool isPathlabHelpCloseButtonAt(
    const sf::Vector2i& position,
    const sf::Vector2u& windowSize
){
    return containsPoint(
        getHelpCloseButtonBounds(windowSize),
        position
    );
}
