#include "PathlabUI.hpp"

#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>

namespace{

// =====================================
// PATHLAB design system
// =====================================

const sf::Color BACKGROUND{
    18,
    20,
    24
};

const sf::Color SURFACE{
    23,
    26,
    32
};

const sf::Color PANEL{
    25,
    28,
    34
};

const sf::Color CONTROL{
    31,
    35,
    43
};

const sf::Color CONTROL_HOVER{
    38,
    43,
    52
};

const sf::Color BORDER{
    48,
    53,
    63
};

const sf::Color TEXT_PRIMARY{
    238,
    240,
    244
};

const sf::Color TEXT_SECONDARY{
    155,
    160,
    170
};

const sf::Color TEXT_MUTED{
    112,
    118,
    130
};

const sf::Color ACCENT{
    59,
    130,
    246
};

const sf::Color ACCENT_HOVER{
    79,
    145,
    255
};

const sf::Color SUCCESS{
    74,
    222,
    128
};

const sf::Color DANGER{
    248,
    113,
    113
};

const sf::Color WARNING{
    250,
    204,
    21
};

// =====================================
// Basic helpers
// =====================================

void drawText(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& text,
    float x,
    float y,
    unsigned int characterSize,
    const sf::Color& color
){
    sf::Text label(font);
    label.setString(text);
    label.setCharacterSize(characterSize);
    label.setPosition(sf::Vector2f(x, y));
    label.setFillColor(color);
    window.draw(label);
}

std::string formatDouble(double value, int precision = 2){
    std::ostringstream stream;

    stream << std::fixed << std::setprecision(precision)<< value;

    return stream.str();
}

void drawDivider(sf::RenderWindow& window, float x, float y, float width){
    sf::RectangleShape divider(sf::Vector2f(width, 1.0f));

    divider.setPosition(sf::Vector2f(x,y));

    divider.setFillColor(BORDER);

    window.draw(divider);
}

void drawSectionHeading(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& text,
    float x,
    float y
){
    drawText(
        window,
        font,
        text,
        x,
        y,
        11,
        TEXT_MUTED
    );
}

void drawValueRow(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& label,
    const std::string& value,
    float panelX,
    float y
){
    drawText(
        window,
        font,
        label,
        panelX + 20.0f,
        y,
        12,
        TEXT_SECONDARY
    );

    sf::Text valueText(font);

    valueText.setString(value);

    valueText.setCharacterSize(12);

    valueText.setFillColor(TEXT_PRIMARY);

    const sf::FloatRect bounds = valueText.getLocalBounds();

    valueText.setPosition(
        sf::Vector2f(
            panelX + PATHLAB_SIDE_PANEL_WIDTH - 20.0f - bounds.size.x,
            y
        )
    );

    window.draw(valueText);
}

sf::FloatRect getRunPlannerButtonBounds(const sf::Vector2u& windowSize){
    const float windowWidth = static_cast<float>(windowSize.x);

    const float windowHeight = static_cast<float>(windowSize.y);

    const float panelX = windowWidth - PATHLAB_SIDE_PANEL_WIDTH;

    const float buttonY = windowHeight - PATHLAB_BOTTOM_BAR_HEIGHT - 56.0f;

    return sf::FloatRect(
        sf::Vector2f(panelX + 20.0f, buttonY),
        sf::Vector2f(PATHLAB_SIDE_PANEL_WIDTH - 40.0f, 40.0f)
    );
}

sf::FloatRect getPlaybackDockBounds(
    const sf::Vector2u& windowSize
){
    const sf::FloatRect runBounds =
        getRunPlannerButtonBounds(
            windowSize
        );


    return sf::FloatRect(
        sf::Vector2f(
            runBounds.position.x,
            runBounds.position.y - 48.0f
        ),
        sf::Vector2f(
            runBounds.size.x,
            38.0f
        )
    );
}

sf::FloatRect getPlaybackResetBounds(
    const sf::Vector2u& windowSize
){
    const sf::FloatRect dock =
        getPlaybackDockBounds(
            windowSize
        );


    return sf::FloatRect(
        sf::Vector2f(
            dock.position.x,
            dock.position.y
        ),
        sf::Vector2f(
            38.0f,
            dock.size.y
        )
    );
}


sf::FloatRect getPlaybackStepBounds(
    const sf::Vector2u& windowSize
){
    const sf::FloatRect dock =
        getPlaybackDockBounds(
            windowSize
        );


    return sf::FloatRect(
        sf::Vector2f(
            dock.position.x
                + dock.size.x
                - 38.0f,

            dock.position.y
        ),
        sf::Vector2f(
            38.0f,
            dock.size.y
        )
    );
}

sf::FloatRect getPlaybackToggleBounds(
    const sf::Vector2u& windowSize
){
    const sf::FloatRect dock =
        getPlaybackDockBounds(
            windowSize
        );


    return sf::FloatRect(
        sf::Vector2f(
            dock.position.x
                + dock.size.x / 2.0f
                - 19.0f,

            dock.position.y
        ),
        sf::Vector2f(
            38.0f,
            dock.size.y
        )
    );
}

sf::FloatRect getPlaybackSpeedBounds(
    const sf::Vector2u& windowSize
){
    const sf::FloatRect dock =
        getPlaybackDockBounds(
            windowSize
        );


    return sf::FloatRect(
        sf::Vector2f(
            dock.position.x
                + dock.size.x
                - 84.0f,

            dock.position.y
        ),
        sf::Vector2f(
            42.0f,
            dock.size.y
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


    constexpr float firstRowY =
        PATHLAB_TOP_BAR_HEIGHT
        + 144.0f;

    constexpr float rowSpacing =
        23.0f;


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


    constexpr float selectorY =
        PATHLAB_TOP_BAR_HEIGHT
        + 58.0f;


    return sf::FloatRect(
        sf::Vector2f(
            panelX + 20.0f,
            selectorY
        ),
        sf::Vector2f(
            PATHLAB_SIDE_PANEL_WIDTH
                - 40.0f,

            32.0f
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

// =====================================
// Checkbox-style visualization row
// =====================================

void drawCheckboxRow(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& label,
    bool checked,
    float panelX,
    float y,
    const sf::Color& previewColor
){
    const float checkboxX =
        panelX + 20.0f;

    const float checkboxY =
        y + 1.0f;


    sf::RectangleShape checkbox(
        sf::Vector2f(
            14.0f,
            14.0f
        )
    );

    checkbox.setPosition(
        sf::Vector2f(
            checkboxX,
            checkboxY
        )
    );

    checkbox.setFillColor(
        checked
            ? ACCENT
            : CONTROL
    );

    checkbox.setOutlineThickness(
        1.0f
    );

    checkbox.setOutlineColor(
        checked
            ? ACCENT
            : BORDER
    );

    window.draw(
        checkbox
    );


    if(checked){

        drawText(
            window,
            font,
            "x",
            checkboxX + 3.0f,
            checkboxY - 3.0f,
            11,
            sf::Color::White
        );
    }


    drawText(
        window,
        font,
        label,
        panelX + 44.0f,
        y,
        12,
        TEXT_SECONDARY
    );


    sf::CircleShape preview(
        4.0f
    );

    preview.setFillColor(
        previewColor
    );

    preview.setPosition(
        sf::Vector2f(
            panelX
                + PATHLAB_SIDE_PANEL_WIDTH
                - 28.0f,

            y + 4.0f
        )
    );

    window.draw(
        preview
    );
}

// =====================================
// Planner status
// =====================================

void drawStatusBadge(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& status,
    float panelX,
    float y
){
    sf::Color statusColor =
        TEXT_MUTED;


    if(status == "Path Found"){

        statusColor =
            SUCCESS;
    }

    else if(status == "No Path"){

        statusColor =
            DANGER;
    }


    sf::CircleShape indicator(
        4.0f
    );

    indicator.setFillColor(
        statusColor
    );

    indicator.setPosition(
        sf::Vector2f(
            panelX + 20.0f,
            y + 5.0f
        )
    );

    window.draw(
        indicator
    );


    drawText(
        window,
        font,
        status,
        panelX + 36.0f,
        y,
        12,
        statusColor
    );
}

// =====================================
// Algorithm selector
// =====================================

void drawAlgorithmSelector(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& algorithm,
    bool dropdownOpen,
    float panelX,
    float y
){
    drawText(
        window,
        font,
        "Algorithm",
        panelX + 20.0f,
        y,
        11,
        TEXT_MUTED
    );


    const sf::FloatRect bounds =
        getAlgorithmSelectorBounds(
            window.getSize()
        );


    const sf::Vector2i mousePosition =
        sf::Mouse::getPosition(
            window
        );


    const bool hovered =
        containsPoint(
            bounds,
            mousePosition
        );


    sf::RectangleShape selector(
        bounds.size
    );

    selector.setPosition(
        bounds.position
    );

    selector.setFillColor(
        hovered
            ? CONTROL_HOVER
            : CONTROL
    );

    selector.setOutlineThickness(
        1.0f
    );

    selector.setOutlineColor(
        dropdownOpen
            ? ACCENT
            : BORDER
    );


    window.draw(
        selector
    );


    drawText(
        window,
        font,
        algorithm,
        bounds.position.x + 11.0f,
        bounds.position.y + 7.0f,
        12,
        TEXT_PRIMARY
    );


    drawText(
        window,
        font,
        dropdownOpen ? "^" : "v",
        bounds.position.x
            + bounds.size.x
            - 20.0f,
        bounds.position.y + 6.0f,
        11,
        TEXT_SECONDARY
    );
}

void drawAlgorithmDropdown(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& selectedAlgorithm
){
    const sf::Vector2i mousePosition =
        sf::Mouse::getPosition(
            window
        );


    const std::string options[2] = {
        "Dijkstra",
        "A*"
    };


    for(std::size_t i = 0; i < 2; ++i){

        const sf::FloatRect bounds =
            getAlgorithmOptionBounds(
                window.getSize(),
                i
            );


        const bool hovered =
            containsPoint(
                bounds,
                mousePosition
            );


        sf::RectangleShape option(
            bounds.size
        );

        option.setPosition(
            bounds.position
        );

        option.setFillColor(
            hovered
                ? CONTROL_HOVER
                : CONTROL
        );

        option.setOutlineThickness(
            1.0f
        );

        option.setOutlineColor(
            BORDER
        );


        window.draw(
            option
        );


        const sf::Color textColor =
            options[i] == selectedAlgorithm
                ? ACCENT
                : TEXT_PRIMARY;


        drawText(
            window,
            font,
            options[i],
            bounds.position.x + 11.0f,
            bounds.position.y + 7.0f,
            12,
            textColor
        );
    }
}

// =====================================
// Playback Dock
// =====================================

void drawPlaybackDock(
    sf::RenderWindow& window,
    const sf::Font& font,
    const PathlabUIData& data
){
    const sf::FloatRect dock =
        getPlaybackDockBounds(
            window.getSize()
        );


    sf::RectangleShape background(
        dock.size
    );

    background.setPosition(
        dock.position
    );

    background.setFillColor(
        CONTROL
    );

    background.setOutlineThickness(
        1.0f
    );

    background.setOutlineColor(
        BORDER
    );

    window.draw(
        background
    );


    // Reset
    drawText(
        window,
        font,
        "|<",
        dock.position.x + 11.0f,
        dock.position.y + 9.0f,
        11,
        TEXT_SECONDARY
    );


    // Progress
    const std::string progress =
        std::to_string(
            data.playbackIndex
        )
        + " / "
        + std::to_string(
            data.playbackTotal
        );


    drawText(
        window,
        font,
        "SEARCH PLAYBACK",
        dock.position.x + 52.0f,
        dock.position.y + 5.0f,
        9,
        TEXT_MUTED
    );


    drawText(
        window,
        font,
        progress,
        dock.position.x + 52.0f,
        dock.position.y + 19.0f,
        10,
        TEXT_PRIMARY
    );

    drawText(
        window,
        font,
        data.playbackPlaying
            ? "||"
            : ">",
        dock.position.x
            + dock.size.x / 2.0f
            - 4.0f,
        dock.position.y + 9.0f,
        11,
        ACCENT
    );

    drawText(
        window,
        font,
        data.playbackSpeed,
        dock.position.x
            + dock.size.x
            - 78.0f,
        dock.position.y + 9.0f,
        10,
        ACCENT
    );

    const float progressRatio =
        data.playbackTotal > 0
            ? static_cast<float>(
                data.playbackIndex
            )
            / static_cast<float>(
                data.playbackTotal
            )
            : 0.0f;


    const float barX =
        dock.position.x + 52.0f;

    const float barY =
        dock.position.y + 31.0f;

    const float barWidth =
        dock.size.x - 150.0f;

    constexpr float barHeight =
        2.0f;


    sf::RectangleShape progressBackground(
        sf::Vector2f(
            barWidth,
            barHeight
        )
    );

    progressBackground.setPosition(
        sf::Vector2f(
            barX,
            barY
        )
    );

    progressBackground.setFillColor(
        BORDER
    );

    window.draw(
        progressBackground
    );


    sf::RectangleShape progressFill(
        sf::Vector2f(
            barWidth * progressRatio,
            barHeight
        )
    );

    progressFill.setPosition(
        sf::Vector2f(
            barX,
            barY
        )
    );

    progressFill.setFillColor(
        ACCENT
    );

    window.draw(
        progressFill
    );


    // Step forward
    drawText(
        window,
        font,
        ">|",
        dock.position.x
            + dock.size.x
            - 27.0f,
        dock.position.y + 9.0f,
        11,
        TEXT_SECONDARY
    );
}

// =====================================
// Run planner button
// =====================================

void drawRunPlannerButton(sf::RenderWindow& window, const sf::Font& font){
    const sf::FloatRect bounds = getRunPlannerButtonBounds(window.getSize());

    const sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

    const bool hovered = containsPoint(bounds, mousePosition);

    sf::RectangleShape button(bounds.size);

    button.setPosition(bounds.position);

    button.setFillColor(hovered ? ACCENT_HOVER : ACCENT);

    window.draw(button);

    drawText(
        window,
        font,
        ">",
        bounds.position.x + 57.0f,
        bounds.position.y + 9.0f,
        13,
        sf::Color::White
    );

    drawText(
        window,
        font,
        "RUN PLANNER",
        bounds.position.x + 76.0f,
        bounds.position.y + 9.0f,
        12,
        sf::Color::White
    );
}

// =====================================
// Top bar
// =====================================

void drawTopBar(
    sf::RenderWindow& window,
    const sf::Font& font
){
    const float windowWidth =
        static_cast<float>(
            window.getSize().x
        );


    sf::RectangleShape background(
        sf::Vector2f(
            windowWidth,
            PATHLAB_TOP_BAR_HEIGHT
        )
    );

    background.setFillColor(
        SURFACE
    );

    window.draw(
        background
    );


    drawText(
        window,
        font,
        "PATHLAB",
        22.0f,
        13.0f,
        20,
        TEXT_PRIMARY
    );


    drawText(
        window,
        font,
        "Path Planning Visualizer",
        120.0f,
        20.0f,
        11,
        TEXT_MUTED
    );


    // ---------------------------------
    // Ready indicator
    // ---------------------------------

    sf::CircleShape readyIndicator(
        4.0f
    );

    readyIndicator.setFillColor(
        SUCCESS
    );

    readyIndicator.setPosition(
        sf::Vector2f(
            windowWidth
                - PATHLAB_SIDE_PANEL_WIDTH
                - 75.0f,

            29.0f
        )
    );

    window.draw(
        readyIndicator
    );


    drawText(
        window,
        font,
        "Ready",
        windowWidth
            - PATHLAB_SIDE_PANEL_WIDTH
            - 60.0f,
        22.0f,
        11,
        TEXT_SECONDARY
    );


    drawDivider(
        window,
        0.0f,
        PATHLAB_TOP_BAR_HEIGHT
            - 1.0f,
        windowWidth
    );
}

// =====================================
// Sidebar
// =====================================

void drawSidePanel(
    sf::RenderWindow& window,
    const sf::Font& font,
    const PathlabUIData& data
){
    const sf::Vector2u windowSize =
        window.getSize();


    const float windowWidth =
        static_cast<float>(
            windowSize.x
        );

    const float windowHeight =
        static_cast<float>(
            windowSize.y
        );


    const float panelX =
        windowWidth
        - PATHLAB_SIDE_PANEL_WIDTH;


    const float panelHeight =
        windowHeight
        - PATHLAB_TOP_BAR_HEIGHT
        - PATHLAB_BOTTOM_BAR_HEIGHT;


    sf::RectangleShape background(
        sf::Vector2f(
            PATHLAB_SIDE_PANEL_WIDTH,
            panelHeight
        )
    );

    background.setPosition(
        sf::Vector2f(
            panelX,
            PATHLAB_TOP_BAR_HEIGHT
        )
    );

    background.setFillColor(
        PANEL
    );

    window.draw(
        background
    );


    // Left border separating canvas / panel.

    sf::RectangleShape panelBorder(
        sf::Vector2f(
            1.0f,
            panelHeight
        )
    );

    panelBorder.setPosition(
        sf::Vector2f(
            panelX,
            PATHLAB_TOP_BAR_HEIGHT
        )
    );

    panelBorder.setFillColor(
        BORDER
    );

    window.draw(
        panelBorder
    );


    float y =
        PATHLAB_TOP_BAR_HEIGHT
        + 16.0f;


    // =====================================
    // Planner
    // =====================================

    drawSectionHeading(
        window,
        font,
        "PLANNER",
        panelX + 20.0f,
        y
    );


    y += 21.0f;


    drawAlgorithmSelector(
        window,
        font,
        data.algorithm,
        data.algorithmDropdownOpen,
        panelX,
        y
    );


    y += 69.0f;


    drawDivider(
        window,
        panelX + 20.0f,
        y,
        PATHLAB_SIDE_PANEL_WIDTH
            - 40.0f
    );


    // =====================================
    // Visualization
    // =====================================

    y += 14.0f;


    drawSectionHeading(
        window,
        font,
        "VISUALIZATION",
        panelX + 20.0f,
        y
    );


    y += 24.0f;


    drawCheckboxRow(
        window,
        font,
        "Obstacles",
        data.showObstacles,
        panelX,
        y,
        sf::Color(
            170,
            175,
            185
        )
    );


    y += 23.0f;


    drawCheckboxRow(
        window,
        font,
        "Visibility Graph",
        data.showVisibilityGraph,
        panelX,
        y,
        sf::Color(
            105,
            110,
            120
        )
    );


    y += 23.0f;


    drawCheckboxRow(
        window,
        font,
        "Final Path",
        data.showFinalPath,
        panelX,
        y,
        WARNING
    );

    if(data.hasSearchTrace){

        y += 23.0f;

        drawCheckboxRow(
            window,
            font,
            "Explored Nodes",
            data.showExploredNodes,
            panelX,
            y,
            sf::Color(
                59,
                130,
                246
            )
        );
    }

    y += 27.0f;

    drawDivider(
        window,
        panelX + 20.0f,
        y,
        PATHLAB_SIDE_PANEL_WIDTH
            - 40.0f
    );

    // =====================================
    // Scene
    // =====================================

    y += 14.0f;

    drawSectionHeading(
        window,
        font,
        "SCENE",
        panelX + 20.0f,
        y
    );

    y += 24.0f;

    drawValueRow(
        window,
        font,
        "Obstacles",
        std::to_string(
            data.obstacleCount
        ),
        panelX,
        y
    );

    y += 21.0f;

    drawValueRow(
        window,
        font,
        "Graph Nodes",
        std::to_string(
            data.graphNodeCount
        ),
        panelX,
        y
    );


    y += 21.0f;


    drawValueRow(
        window,
        font,
        "Graph Edges",
        std::to_string(
            data.graphEdgeCount
        ),
        panelX,
        y
    );


    y += 27.0f;


    drawDivider(
        window,
        panelX + 20.0f,
        y,
        PATHLAB_SIDE_PANEL_WIDTH
            - 40.0f
    );


    // =====================================
    // Results
    // =====================================

    y += 14.0f;


    drawSectionHeading(
        window,
        font,
        "RESULTS",
        panelX + 20.0f,
        y
    );


    y += 24.0f;


    drawStatusBadge(
        window,
        font,
        data.plannerStatus,
        panelX,
        y
    );


    y += 24.0f;


    drawValueRow(
        window,
        font,
        "Path Length",
        formatDouble(
            data.pathLength
        ) + " px",
        panelX,
        y
    );


    y += 21.0f;


    drawValueRow(
        window,
        font,
        "Path Segments",
        std::to_string(
            data.pathSegments
        ),
        panelX,
        y
    );


    y += 21.0f;


    drawValueRow(
        window,
        font,
        "Nodes in Path",
        std::to_string(
            data.pathNodes
        ),
        panelX,
        y
    );

    y += 21.0f;


    drawValueRow(
        window,
        font,
        "Nodes Expanded",
        std::to_string(
            data.nodesExpanded
        ),
        panelX,
        y
    );


    y += 27.0f;


    drawDivider(
        window,
        panelX + 20.0f,
        y,
        PATHLAB_SIDE_PANEL_WIDTH
            - 40.0f
    );


    // =====================================
    // Performance
    // =====================================

    y += 14.0f;


    drawSectionHeading(
        window,
        font,
        "PERFORMANCE",
        panelX + 20.0f,
        y
    );


    y += 24.0f;


    drawValueRow(
        window,
        font,
        "Graph Build",
        formatDouble(
            data.graphBuildTimeMs
        ) + " ms",
        panelX,
        y
    );


    y += 21.0f;


    drawValueRow(
        window,
        font,
        "Search",
        formatDouble(
            data.searchTimeMs
        ) + " ms",
        panelX,
        y
    );


    y += 21.0f;


    drawValueRow(
        window,
        font,
        "Total",
        formatDouble(
            data.totalTimeMs
        ) + " ms",
        panelX,
        y
    );


    // =====================================
    // Run button
    // =====================================

    drawRunPlannerButton(
        window,
        font
    );
}


// =====================================
// Bottom shortcut bar
// =====================================

void drawShortcut(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& key,
    const std::string& description,
    float x,
    float y
){
    sf::Text keyText(font);

    keyText.setString(
        key
    );

    keyText.setCharacterSize(
        10
    );

    keyText.setFillColor(
        TEXT_PRIMARY
    );


    const sf::FloatRect bounds =
        keyText.getLocalBounds();


    const float chipWidth =
        bounds.size.x + 12.0f;


    sf::RectangleShape chip(
        sf::Vector2f(
            chipWidth,
            20.0f
        )
    );

    chip.setPosition(
        sf::Vector2f(
            x,
            y
        )
    );

    chip.setFillColor(
        CONTROL
    );

    chip.setOutlineThickness(
        1.0f
    );

    chip.setOutlineColor(
        BORDER
    );

    window.draw(
        chip
    );


    keyText.setPosition(
        sf::Vector2f(
            x + 6.0f,
            y + 3.0f
        )
    );

    window.draw(
        keyText
    );


    drawText(
        window,
        font,
        description,
        x + chipWidth + 7.0f,
        y + 3.0f,
        10,
        TEXT_MUTED
    );
}


void drawBottomBar(
    sf::RenderWindow& window,
    const sf::Font& font
){
    const sf::Vector2u windowSize =
        window.getSize();


    const float windowWidth =
        static_cast<float>(
            windowSize.x
        );

    const float windowHeight =
        static_cast<float>(
            windowSize.y
        );


    const float barY =
        windowHeight
        - PATHLAB_BOTTOM_BAR_HEIGHT;


    sf::RectangleShape background(
        sf::Vector2f(
            windowWidth,
            PATHLAB_BOTTOM_BAR_HEIGHT
        )
    );

    background.setPosition(
        sf::Vector2f(
            0.0f,
            barY
        )
    );

    background.setFillColor(
        SURFACE
    );

    window.draw(
        background
    );


    drawDivider(
        window,
        0.0f,
        barY,
        windowWidth
    );


    const float shortcutY =
        barY + 14.0f;


    drawShortcut(
        window,
        font,
        "LMB",
        "Add Vertex",
        18.0f,
        shortcutY
    );


    drawShortcut(
        window,
        font,
        "S",
        "Set Start",
        145.0f,
        shortcutY
    );


    drawShortcut(
        window,
        font,
        "G",
        "Set Goal",
        245.0f,
        shortcutY
    );


    drawShortcut(
        window,
        font,
        "SPACE",
        "Run Planner",
        345.0f,
        shortcutY
    );


    drawShortcut(
        window,
        font,
        "V",
        "Graph",
        500.0f,
        shortcutY
    );


    drawShortcut(
        window,
        font,
        "R",
        "Reset",
        585.0f,
        shortcutY
    );
}

}

bool loadPathlabFont(sf::Font& font){
    if(!font.openFromFile(
        "assets/fonts/InterVariable.ttf"
    )){
        std::cerr
            << "Failed to load PATHLAB font."
            << std::endl;

        return false;
    }

    return true;
}

void drawPathlabUI(
    sf::RenderWindow& window, 
    const sf::Font& font,
    const PathlabUIData& data
){
    drawTopBar(window, font);

    drawSidePanel(window, font, data);

    if(data.algorithmDropdownOpen){

        drawAlgorithmDropdown(
            window,
            font,
            data.algorithm
        );
    }

    if(data.hasSearchTrace){

        drawPlaybackDock(
            window,
            font,
            data
        );
    }

    drawBottomBar(window, font);
}

PathlabUIAction handlePathlabUIClick(
    const sf::Vector2i& position,
    const sf::Vector2u& windowSize,
    bool algorithmDropdownOpen,
    bool hasSearchTrace
){
    const sf::FloatRect selectorBounds =
        getAlgorithmSelectorBounds(
            windowSize
        );

    // =====================================
    // Open dropdown options
    // =====================================

    if(algorithmDropdownOpen){

        const sf::FloatRect dijkstraBounds =
            getAlgorithmOptionBounds(
                windowSize,
                0
            );

        if(containsPoint(
            dijkstraBounds,
            position
        )){
            return
                PathlabUIAction::SelectDijkstra;
        }

        const sf::FloatRect aStarBounds =
            getAlgorithmOptionBounds(
                windowSize,
                1
            );

        if(containsPoint(
            aStarBounds,
            position
        )){
            return
                PathlabUIAction::SelectAStar;
        }
    }

    // =====================================
    // Algorithm selector
    // =====================================

    if(containsPoint(
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

    if(containsPoint(
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

    if(containsPoint(
        obstaclesBounds,
        position
    )){
        return
            PathlabUIAction::ToggleObstacles;
    }

    const sf::FloatRect visibilityGraphBounds =
        getVisualizationRowBounds(
            windowSize,
            1
        );

    if(containsPoint(
        visibilityGraphBounds,
        position
    )){
        return
            PathlabUIAction::ToggleVisibilityGraph;
    }

    const sf::FloatRect finalPathBounds =
        getVisualizationRowBounds(
            windowSize,
            2
        );

    if(containsPoint(
        finalPathBounds,
        position
    )){
        return
            PathlabUIAction::ToggleFinalPath;
    }

    if(hasSearchTrace){
        const sf::FloatRect exploredNodesBounds =
            getVisualizationRowBounds(
                windowSize,
                3
            );

        if(containsPoint(exploredNodesBounds,position)){
            return PathlabUIAction::ToggleExploredNodes;
        }
    }

    if(hasSearchTrace){

        const sf::FloatRect resetBounds =
            getPlaybackResetBounds(
                windowSize
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
                windowSize
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
                windowSize
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
                windowSize
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

    if(algorithmDropdownOpen){

        return
            PathlabUIAction::CloseAlgorithmDropdown;
    }

    return
        PathlabUIAction::None;
}

