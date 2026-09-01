#include "PathlabUI.hpp"

#include "PathlabUILayout.hpp"

using namespace pathlab_ui_detail;

PathlabUIInteraction handlePathlabUIClick(
    const sf::Vector2i& position,
    const sf::Vector2u& windowSize,
    const PathlabUIData& data
){
    if(containsPoint(getHelpButtonBounds(windowSize), position)){
        return PathlabUIAction::OpenHelpOverlay;
    }

    if(containsPoint(
        getSidebarToggleButtonBounds(windowSize),
        position
    )){
        return PathlabUIAction::ToggleSidebar;
    }

    if(containsPoint(
        getLoadDemoSceneButtonBounds(windowSize, data.sidebarVisible),
        position
    )){
        return PathlabUIAction::LoadDemoScene;
    }

    if(containsPoint(
        getResetCameraButtonBounds(windowSize, data.sidebarVisible),
        position
    )){
        return PathlabUIAction::ResetCamera;
    }

    const sf::FloatRect selectorBounds =
        getAlgorithmSelectorBounds(
            windowSize
        );

    // =====================================
    // Open dropdown options
    // =====================================

    if(data.sidebarVisible && data.algorithmDropdownOpen){

        for(std::size_t i = 0; i < data.algorithmOptions.size(); ++i){
            const sf::FloatRect optionBounds =
                getAlgorithmOptionBounds(
                    windowSize,
                    i
                );

            if(containsPoint(
                optionBounds,
                position
            )){
                return {
                    PathlabUIAction::SelectAlgorithm,
                    i
                };
            }
        }
    }

    // =====================================
    // Algorithm selector
    // =====================================

    if(data.sidebarVisible && containsPoint(
        selectorBounds,
        position
    )){
        return
            PathlabUIAction::ToggleAlgorithmDropdown;
    }

    // =====================================
    // Run planner
    // =====================================

    const sf::FloatRect runPlannerBounds =
        getRunPlannerButtonBounds(
            windowSize
        );

    if(data.sidebarVisible && containsPoint(
        runPlannerBounds,
        position
    )){
        return
            PathlabUIAction::RunPlanner;
    }

    // =====================================
    // Visualization rows
    // =====================================

    const sf::FloatRect obstaclesBounds =
        getVisualizationRowBounds(
            windowSize,
            0
        );

    if(data.sidebarVisible && containsPoint(
        obstaclesBounds,
        position
    )){
        return data.hasObstacles
            ? PathlabUIAction::ToggleObstacles
            : data.algorithmDropdownOpen
                ? PathlabUIAction::CloseAlgorithmDropdown
                : PathlabUIAction::None;
    }

    const sf::FloatRect visibilityGraphBounds =
        getVisualizationRowBounds(
            windowSize,
            1
        );

    if(data.sidebarVisible && containsPoint(
        visibilityGraphBounds,
        position
    )){
        return data.hasVisibilityGraph
            ? PathlabUIAction::ToggleVisibilityGraph
            : data.algorithmDropdownOpen
                ? PathlabUIAction::CloseAlgorithmDropdown
                : PathlabUIAction::None;
    }

    const sf::FloatRect finalPathBounds =
        getVisualizationRowBounds(
            windowSize,
            2
        );

    if(data.sidebarVisible && containsPoint(
        finalPathBounds,
        position
    )){
        return data.hasFinalPath
            ? PathlabUIAction::ToggleFinalPath
            : data.algorithmDropdownOpen
                ? PathlabUIAction::CloseAlgorithmDropdown
                : PathlabUIAction::None;
    }

    if(data.sidebarVisible && data.hasSearchTrace){
        const sf::FloatRect exploredNodesBounds =
            getVisualizationRowBounds(
                windowSize,
                3
            );

        if(containsPoint(exploredNodesBounds,position)){
            return PathlabUIAction::ToggleExploredNodes;
        }
    }

    if(data.hasSearchTrace){

        const sf::FloatRect resetBounds =
            getPlaybackResetBounds(
                windowSize,
                data.sidebarVisible
            );


        if(containsPoint(
            resetBounds,
            position
        )){
            return
                PathlabUIAction::ResetPlayback;
        }

        const sf::FloatRect toggleBounds =
            getPlaybackToggleBounds(
                windowSize,
                data.sidebarVisible
            );


        if(containsPoint(
            toggleBounds,
            position
        )){
            return
                PathlabUIAction::TogglePlayback;
        }

        const sf::FloatRect speedBounds =
            getPlaybackSpeedBounds(
                windowSize,
                data.sidebarVisible
            );


        if(containsPoint(
            speedBounds,
            position
        )){
            return
                PathlabUIAction::CyclePlaybackSpeed;
        }


        const sf::FloatRect stepBounds =
            getPlaybackStepBounds(
                windowSize,
                data.sidebarVisible
            );


        if(containsPoint(
            stepBounds,
            position
        )){
            return
                PathlabUIAction::StepPlayback;
        }
    }


    // Clicking anywhere else closes
    // an open dropdown.

    if(data.algorithmDropdownOpen){

        return
            PathlabUIAction::CloseAlgorithmDropdown;
    }

    return
        PathlabUIAction::None;
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
