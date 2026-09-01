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


    const float selectorY =
        y + 21.0f;


    sf::RectangleShape selector(
        sf::Vector2f(
            PATHLAB_SIDE_PANEL_WIDTH
                - 40.0f,

            32.0f
        )
    );

    selector.setPosition(
        sf::Vector2f(
            panelX + 20.0f,
            selectorY
        )
    );

    selector.setFillColor(
        CONTROL
    );

    selector.setOutlineThickness(
        1.0f
    );

    selector.setOutlineColor(
        BORDER
    );

    window.draw(
        selector
    );


    drawText(
        window,
        font,
        algorithm,
        panelX + 31.0f,
        selectorY + 7.0f,
        12,
        TEXT_PRIMARY
    );


    drawText(
        window,
        font,
        "v",
        panelX
            + PATHLAB_SIDE_PANEL_WIDTH
            - 40.0f,
        selectorY + 6.0f,
        11,
        TEXT_SECONDARY
    );
}

// =====================================
// Run planner button
// =====================================

void drawRunPlannerButton(
    sf::RenderWindow& window,
    const sf::Font& font,
    float panelX,
    float y
){
    sf::RectangleShape button(
        sf::Vector2f(
            PATHLAB_SIDE_PANEL_WIDTH
                - 40.0f,

            40.0f
        )
    );

    button.setPosition(
        sf::Vector2f(
            panelX + 20.0f,
            y
        )
    );

    button.setFillColor(
        ACCENT
    );

    window.draw(
        button
    );


    drawText(
        window,
        font,
        ">",
        panelX + 77.0f,
        y + 9.0f,
        13,
        sf::Color::White
    );


    drawText(
        window,
        font,
        "RUN PLANNER",
        panelX + 96.0f,
        y + 9.0f,
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

    const float runButtonY =
        windowHeight
        - PATHLAB_BOTTOM_BAR_HEIGHT
        - 56.0f;


    drawRunPlannerButton(
        window,
        font,
        panelX,
        runButtonY
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

    drawBottomBar(window, font);
}

