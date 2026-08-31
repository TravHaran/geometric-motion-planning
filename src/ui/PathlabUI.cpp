#include "PathlabUI.hpp"

#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>

namespace{

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

void drawDivider(sf::RenderWindow& window, float x, float y, float width){
    sf::RectangleShape divider(sf::Vector2f(width, 1.0f));

    divider.setPosition(sf::Vector2f(x,y));

    divider.setFillColor(sf::Color(48,52,60));

    window.draw(divider);
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
        sf::Color(
            135,
            140,
            150
        )
    );

    sf::Text valueText(font);

    valueText.setString(value);

    valueText.setCharacterSize(12);

    valueText.setFillColor(sf::Color(225,228,235));

    const sf::FloatRect bounds = valueText.getLocalBounds();

    valueText.setPosition(
        sf::Vector2f(
            panelX + PATHLAB_SIDE_PANEL_WIDTH - 20.0f - bounds.size.x,
            y
        )
    );

    window.draw(valueText);
}

std::string formatDouble(double value, int precision = 2){
    std::ostringstream stream;

    stream << std::fixed << std::setprecision(precision)<< value;

    return stream.str();
}

void drawTopBar(sf::RenderWindow& window, const sf::Font& font){
    const float windowWidth = static_cast<float>(window.getSize().x);

    sf::RectangleShape background(sf::Vector2f(windowWidth, PATHLAB_TOP_BAR_HEIGHT));

    background.setFillColor(sf::Color(23, 26, 32));

    window.draw(background);

    drawText(
        window, 
        font, 
        "PATHLAB", 
        22.0f,
        14.0f,
        21,
        sf::Color(240, 242, 245)
    );

    drawText(
        window, 
        font, 
        "Path Planning Visualizer", 
        125.0f,
        21.0f,
        12,
        sf::Color(140, 145, 155)
    );
}

void drawSidePanel(
    sf::RenderWindow& window, 
    const sf::Font& font,
    const PathlabUIData& data
){
    const sf::Vector2u windowSize = window.getSize();

    const float windowWidth = static_cast<float>(windowSize.x);
    const float windowHeight = static_cast<float>(windowSize.y);

    const float panelX = windowWidth - PATHLAB_SIDE_PANEL_WIDTH;
    const float panelHeight = windowHeight - PATHLAB_TOP_BAR_HEIGHT - PATHLAB_BOTTOM_BAR_HEIGHT;

    sf::RectangleShape background(sf::Vector2f(PATHLAB_SIDE_PANEL_WIDTH, panelHeight));

    background.setPosition(sf::Vector2f(panelX, PATHLAB_TOP_BAR_HEIGHT));

    background.setFillColor(sf::Color(25, 28, 34));

    window.draw(background);

    // =====================================
    // Planner
    // =====================================

    float y = PATHLAB_TOP_BAR_HEIGHT + 20.0f;

    drawText(
        window,
        font, 
        "PLANNER",
        panelX + 20.0f,
        PATHLAB_TOP_BAR_HEIGHT + 22.0f,
        13,
        sf::Color(190, 195, 205)
    );

    y += 32.0f;

    drawValueRow(
        window,
        font,
        "Algorithm",
        data.algorithm,
        panelX,
        y
    );

    y += 34.0f;

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

    y += 20.0f;

    drawText(
        window,
        font,
        "VISUALIZATION",
        panelX + 20.0f,
        y,
        13,
        sf::Color(
            190,
            195,
            205
        )
    );

    y += 30.0f;

    drawValueRow(
        window,
        font,
        "Obstacles",
        data.showObstacles
            ? "Visible"
            : "Hidden",
        panelX,
        y
    );

    y += 23.0f;

    drawValueRow(
        window,
        font,
        "Visibility Graph",
        data.showVisibilityGraph
            ? "Visible"
            : "Hidden",
        panelX,
        y
    );

    y += 23.0f;

    drawValueRow(
        window,
        font,
        "Final Path",
        data.showFinalPath
            ? "Visible"
            : "Hidden",
        panelX,
        y
    );

    y += 34.0f;

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

    y += 20.0f;

    drawText(
        window,
        font,
        "SCENE",
        panelX + 20.0f,
        y,
        13,
        sf::Color(
            190,
            195,
            205
        )
    );

    y += 30.0f;

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

    y += 23.0f;

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

    y += 23.0f;

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

    y += 34.0f;

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

    y += 20.0f;

    drawText(
        window,
        font,
        "RESULTS",
        panelX + 20.0f,
        y,
        13,
        sf::Color(
            190,
            195,
            205
        )
    );

    y += 30.0f;

    drawValueRow(
        window,
        font,
        "Status",
        data.plannerStatus,
        panelX,
        y
    );

    y += 23.0f;

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

    y += 23.0f;

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

    y += 23.0f;

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

    y += 34.0f;

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

    y += 20.0f;

    drawText(
        window,
        font,
        "PERFORMANCE",
        panelX + 20.0f,
        y,
        13,
        sf::Color(
            190,
            195,
            205
        )
    );

    y += 30.0f;

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

    y += 23.0f;

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

    y += 23.0f;

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
}

void drawBottomBar(sf::RenderWindow& window, const sf::Font& font){
    const sf::Vector2u windowSize = window.getSize();
    const float windowWidth = static_cast<float>(windowSize.x);
    const float windowHeight = static_cast<float>(windowSize.y);
    const float barY = windowHeight - PATHLAB_BOTTOM_BAR_HEIGHT;

    sf::RectangleShape background(sf::Vector2f(windowWidth, PATHLAB_BOTTOM_BAR_HEIGHT));

    background.setPosition(sf::Vector2f(0.0f,barY));

    background.setFillColor(sf::Color(23,26,32));

    window.draw(background);

    drawText(
        window,
        font,
        "LMB  Add Vertex",
        20.0f,
        barY + 15.0f,
        11,
        sf::Color(
            165,
            170,
            180
        )
    );

    drawText(
        window,
        font,
        "S  Set Start",
        150.0f,
        barY + 15.0f,
        11,
        sf::Color(
            165,
            170,
            180
        )
    );

    drawText(
        window,
        font,
        "G  Set Goal",
        255.0f,
        barY + 15.0f,
        11,
        sf::Color(
            165,
            170,
            180
        )
    );

    drawText(
        window,
        font,
        "SPACE  Run Planner",
        360.0f,
        barY + 15.0f,
        11,
        sf::Color(
            165,
            170,
            180
        )
    );

    drawText(
        window,
        font,
        "R  Reset",
        515.0f,
        barY + 15.0f,
        11,
        sf::Color(
            165,
            170,
            180
        )
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

