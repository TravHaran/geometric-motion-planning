#include "PathlabUILayout.hpp"

#include "PathlabUI.hpp"

#include <algorithm>

namespace pathlab_ui_detail{

bool useCompactSidebarLayout(const sf::Vector2u& windowSize){
    return windowSize.y < 780;
}

float getUsableCanvasWidth(
    const sf::Vector2u& windowSize,
    bool sidebarVisible
){
    const float sidebarWidth =
        sidebarVisible ? PATHLAB_SIDE_PANEL_WIDTH : 0.0f;

    return std::max(
        1.0f,
        static_cast<float>(windowSize.x) - sidebarWidth
    );
}

sf::FloatRect getRunPlannerButtonBounds(const sf::Vector2u& windowSize){
    const float windowWidth = static_cast<float>(windowSize.x);

    const float windowHeight = static_cast<float>(windowSize.y);

    const float panelX = windowWidth - PATHLAB_SIDE_PANEL_WIDTH;

    const float bottomInset =
        useCompactSidebarLayout(windowSize)
            ? 52.0f
            : 56.0f;

    const float buttonY =
        windowHeight
        - PATHLAB_BOTTOM_BAR_HEIGHT
        - bottomInset;

    return sf::FloatRect(
        sf::Vector2f(panelX + 20.0f, buttonY),
        sf::Vector2f(PATHLAB_SIDE_PANEL_WIDTH - 40.0f, 40.0f)
    );
}

sf::FloatRect getResetCameraButtonBounds(
    const sf::Vector2u& windowSize,
    bool sidebarVisible
){
    const float canvasWidth =
        getUsableCanvasWidth(windowSize, sidebarVisible);

    return sf::FloatRect(
        sf::Vector2f(
            std::max(14.0f, canvasWidth - 130.0f),
            PATHLAB_TOP_BAR_HEIGHT + 14.0f
        ),
        sf::Vector2f(116.0f, 32.0f)
    );
}

sf::FloatRect getLoadDemoSceneButtonBounds(
    const sf::Vector2u& windowSize,
    bool sidebarVisible
){
    const sf::FloatRect resetCameraBounds =
        getResetCameraButtonBounds(windowSize, sidebarVisible);

    return sf::FloatRect(
        sf::Vector2f(
            resetCameraBounds.position.x - 132.0f,
            resetCameraBounds.position.y
        ),
        sf::Vector2f(124.0f, resetCameraBounds.size.y)
    );
}

sf::FloatRect getPlaybackDockBounds(
    const sf::Vector2u& windowSize,
    bool sidebarVisible
){
    constexpr float preferredWidth = 440.0f;
    constexpr float horizontalMargin = 16.0f;
    constexpr float dockHeight = 78.0f;
    constexpr float bottomGap = 24.0f;

    const float windowHeight =
        static_cast<float>(windowSize.y);

    const float canvasWidth =
        getUsableCanvasWidth(windowSize, sidebarVisible);

    const float dockWidth =
        std::min(
            preferredWidth,
            std::max(
                0.0f,
                canvasWidth - horizontalMargin * 2.0f
            )
        );


    return sf::FloatRect(
        sf::Vector2f(
            (canvasWidth - dockWidth) / 2.0f,
            windowHeight
                - PATHLAB_BOTTOM_BAR_HEIGHT
                - bottomGap
                - dockHeight
        ),
        sf::Vector2f(
            dockWidth,
            dockHeight
        )
    );
}

sf::FloatRect getPlaybackResetBounds(
    const sf::Vector2u& windowSize,
    bool sidebarVisible
){
    const sf::FloatRect dock =
        getPlaybackDockBounds(
            windowSize,
            sidebarVisible
        );


    return sf::FloatRect(
        sf::Vector2f(
            dock.position.x + 14.0f,
            dock.position.y + 12.0f
        ),
        sf::Vector2f(
            44.0f,
            28.0f
        )
    );
}


sf::FloatRect getPlaybackStepBounds(
    const sf::Vector2u& windowSize,
    bool sidebarVisible
){
    const sf::FloatRect dock =
        getPlaybackDockBounds(
            windowSize,
            sidebarVisible
        );


    return sf::FloatRect(
        sf::Vector2f(
            dock.position.x + 122.0f,
            dock.position.y + 12.0f
        ),
        sf::Vector2f(
            44.0f,
            28.0f
        )
    );
}

sf::FloatRect getPlaybackToggleBounds(
    const sf::Vector2u& windowSize,
    bool sidebarVisible
){
    const sf::FloatRect dock =
        getPlaybackDockBounds(
            windowSize,
            sidebarVisible
        );


    return sf::FloatRect(
        sf::Vector2f(
            dock.position.x + 68.0f,
            dock.position.y + 12.0f
        ),
        sf::Vector2f(
            44.0f,
            28.0f
        )
    );
}

sf::FloatRect getPlaybackSpeedBounds(
    const sf::Vector2u& windowSize,
    bool sidebarVisible
){
    const sf::FloatRect dock =
        getPlaybackDockBounds(
            windowSize,
            sidebarVisible
        );


    return sf::FloatRect(
        sf::Vector2f(
            dock.position.x + 178.0f,
            dock.position.y + 12.0f
        ),
        sf::Vector2f(
            58.0f,
            28.0f
        )
    );
}

sf::FloatRect getVisualizationRowBounds(
    const sf::Vector2u& windowSize,
    std::size_t rowIndex
){
    const float windowWidth =
        static_cast<float>(
            windowSize.x
        );

    const float panelX =
        windowWidth
        - PATHLAB_SIDE_PANEL_WIDTH;


    const bool compact =
        useCompactSidebarLayout(windowSize);

    const float firstRowY =
        PATHLAB_TOP_BAR_HEIGHT
        + (compact ? 105.0f : 144.0f);

    const float rowSpacing =
        compact ? 19.0f : 23.0f;


    const float rowY =
        firstRowY
        + rowSpacing
            * static_cast<float>(
                rowIndex
            );


    return sf::FloatRect(
        sf::Vector2f(
            panelX + 14.0f,
            rowY - 3.0f
        ),
        sf::Vector2f(
            PATHLAB_SIDE_PANEL_WIDTH
                - 28.0f,

            21.0f
        )
    );
}

sf::FloatRect getAlgorithmSelectorBounds(
    const sf::Vector2u& windowSize
){
    const float windowWidth =
        static_cast<float>(
            windowSize.x
        );

    const float panelX =
        windowWidth
        - PATHLAB_SIDE_PANEL_WIDTH;


    const bool compact =
        useCompactSidebarLayout(windowSize);

    const float selectorY =
        PATHLAB_TOP_BAR_HEIGHT
        + (compact ? 40.0f : 58.0f);


    return sf::FloatRect(
        sf::Vector2f(
            panelX + 20.0f,
            selectorY
        ),
        sf::Vector2f(
            PATHLAB_SIDE_PANEL_WIDTH
                - 40.0f,

            compact ? 30.0f : 32.0f
        )
    );
}


sf::FloatRect getAlgorithmOptionBounds(
    const sf::Vector2u& windowSize,
    std::size_t optionIndex
){
    const sf::FloatRect selectorBounds =
        getAlgorithmSelectorBounds(
            windowSize
        );


    constexpr float optionHeight =
        32.0f;


    return sf::FloatRect(
        sf::Vector2f(
            selectorBounds.position.x,

            selectorBounds.position.y
                + selectorBounds.size.y
                + optionHeight
                    * static_cast<float>(
                        optionIndex
                    )
        ),
        sf::Vector2f(
            selectorBounds.size.x,
            optionHeight
        )
    );
}

bool containsPoint(const sf::FloatRect& bounds, const sf::Vector2i& position){
    const float x = static_cast<float>(position.x);

    const float y = static_cast<float>(position.y);

    return
        x >= bounds.position.x &&
        x < bounds.position.x + bounds.size.x &&
        y >= bounds.position.y &&
        y < bounds.position.y + bounds.size.y;
}

sf::FloatRect getHelpPanelBounds(const sf::Vector2u& windowSize){
    constexpr float preferredWidth = 820.0f;
    constexpr float preferredHeight = 560.0f;
    constexpr float screenMargin = 20.0f;

    const float windowWidth = static_cast<float>(windowSize.x);
    const float windowHeight = static_cast<float>(windowSize.y);

    const float panelWidth =
        std::min(preferredWidth, std::max(0.0f, windowWidth - 2.0f * screenMargin));

    const float panelHeight =
        std::min(preferredHeight, std::max(0.0f, windowHeight - 2.0f * screenMargin));

    return sf::FloatRect(
        sf::Vector2f(
            (windowWidth - panelWidth) / 2.0f,
            (windowHeight - panelHeight) / 2.0f
        ),
        sf::Vector2f(panelWidth, panelHeight)
    );
}

sf::FloatRect getHelpButtonBounds(const sf::Vector2u& windowSize){
    const float windowWidth = static_cast<float>(windowSize.x);

    return sf::FloatRect(
        sf::Vector2f(windowWidth - 50.0f, 15.0f),
        sf::Vector2f(34.0f, 34.0f)
    );
}

sf::FloatRect getSidebarToggleButtonBounds(
    const sf::Vector2u& windowSize
){
    return sf::FloatRect(
        sf::Vector2f(
            std::max(
                14.0f,
                static_cast<float>(windowSize.x) - 92.0f
            ),
            15.0f
        ),
        sf::Vector2f(34.0f, 34.0f)
    );
}

sf::FloatRect getHelpCloseButtonBounds(const sf::Vector2u& windowSize){
    const sf::FloatRect panelBounds = getHelpPanelBounds(windowSize);

    return sf::FloatRect(
        sf::Vector2f(
            panelBounds.position.x + panelBounds.size.x - 50.0f,
            panelBounds.position.y + 16.0f
        ),
        sf::Vector2f(34.0f, 34.0f)
    );
}
