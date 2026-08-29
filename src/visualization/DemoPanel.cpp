#include "DemoPanel.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>


namespace{


constexpr float SELECTOR_X_OFFSET =
    20.0f;

constexpr float SELECTOR_Y =
    402.0f;

constexpr float SELECTOR_WIDTH =
    220.0f;

constexpr float SELECTOR_HEIGHT =
    28.0f;


void drawPanelText(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& text,
    float x,
    float y,
    unsigned int characterSize,
    const sf::Color& color
){
    sf::Text label(font);

    label.setString(
        text
    );

    label.setCharacterSize(
        characterSize
    );

    label.setPosition(
        sf::Vector2f(
            x,
            y
        )
    );

    label.setFillColor(
        color
    );

    window.draw(
        label
    );
}


void drawStatusRow(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& label,
    const std::string& value,
    float panelX,
    float y
){
    const sf::Color labelColor(
        150,
        150,
        160
    );

    const sf::Color valueColor(
        235,
        235,
        240
    );


    drawPanelText(
        window,
        font,
        label,
        panelX + 20.0f,
        y,
        12,
        labelColor
    );


    drawPanelText(
        window,
        font,
        value,
        panelX + 125.0f,
        y,
        12,
        valueColor
    );
}


std::string formatDouble(
    double value,
    int precision
){
    std::ostringstream stream;

    stream
        << std::fixed
        << std::setprecision(
            precision
        )
        << value;

    return stream.str();
}


bool pointInsideRectangle(
    const sf::Vector2i& point,
    float x,
    float y,
    float width,
    float height
){
    const float px =
        static_cast<float>(
            point.x
        );

    const float py =
        static_cast<float>(
            point.y
        );

    return
        px >= x
        &&
        px <= x + width
        &&
        py >= y
        &&
        py <= y + height;
}


void drawAlgorithmSelector(
    sf::RenderWindow& window,
    const sf::Font& font,
    const DemoPanelData& data,
    float panelX
){
    const sf::Color boxColor(
        42,
        42,
        48
    );

    const sf::Color hoverBorderColor(
        85,
        85,
        95
    );

    const sf::Color textColor(
        235,
        235,
        240
    );

    const sf::Color secondaryColor(
        150,
        150,
        160
    );


    // =====================================
    // Main selector box
    // =====================================

    sf::RectangleShape selector(
        sf::Vector2f(
            SELECTOR_WIDTH,
            SELECTOR_HEIGHT
        )
    );

    selector.setPosition(
        sf::Vector2f(
            panelX + SELECTOR_X_OFFSET,
            SELECTOR_Y
        )
    );

    selector.setFillColor(
        boxColor
    );

    selector.setOutlineThickness(
        1.0f
    );

    selector.setOutlineColor(
        hoverBorderColor
    );

    window.draw(
        selector
    );


    drawPanelText(
        window,
        font,
        data.algorithm,
        panelX + 30.0f,
        SELECTOR_Y + 6.0f,
        12,
        textColor
    );


    drawPanelText(
        window,
        font,
        data.algorithmDropdownOpen
            ? "^"
            : "v",
        panelX + 220.0f,
        SELECTOR_Y + 5.0f,
        12,
        secondaryColor
    );


    // =====================================
    // Dropdown options
    // =====================================

    if(!data.algorithmDropdownOpen){
        return;
    }


    sf::RectangleShape dijkstraOption(
        sf::Vector2f(
            SELECTOR_WIDTH,
            SELECTOR_HEIGHT
        )
    );

    dijkstraOption.setPosition(
        sf::Vector2f(
            panelX + SELECTOR_X_OFFSET,
            SELECTOR_Y
            + SELECTOR_HEIGHT
        )
    );

    dijkstraOption.setFillColor(
        sf::Color(
            48,
            48,
            55
        )
    );

    window.draw(
        dijkstraOption
    );


    drawPanelText(
        window,
        font,
        "Dijkstra",
        panelX + 30.0f,
        SELECTOR_Y
            + SELECTOR_HEIGHT
            + 6.0f,
        12,
        textColor
    );


    sf::RectangleShape aStarOption(
        sf::Vector2f(
            SELECTOR_WIDTH,
            SELECTOR_HEIGHT
        )
    );

    aStarOption.setPosition(
        sf::Vector2f(
            panelX + SELECTOR_X_OFFSET,
            SELECTOR_Y
            + 2.0f * SELECTOR_HEIGHT
        )
    );

    aStarOption.setFillColor(
        sf::Color(
            48,
            48,
            55
        )
    );

    window.draw(
        aStarOption
    );


    drawPanelText(
        window,
        font,
        "A*",
        panelX + 30.0f,
        SELECTOR_Y
            + 2.0f * SELECTOR_HEIGHT
            + 6.0f,
        12,
        textColor
    );
}


}


bool loadDemoPanelFont(
    sf::Font& font
){
    if(!font.openFromFile(
        "assets/fonts/InterVariable.ttf"
    )){
        std::cerr
            << "Failed to load demo panel font."
            << std::endl;

        return false;
    }

    return true;
}


AlgorithmSelectorAction getAlgorithmSelectorAction(
    const sf::Vector2i& mousePosition,
    const sf::Vector2u& windowSize,
    bool dropdownOpen
){
    const float panelX =
        static_cast<float>(
            windowSize.x
        )
        - DEMO_PANEL_WIDTH;


    // =====================================
    // Main selector
    // =====================================

    if(pointInsideRectangle(
        mousePosition,
        panelX + SELECTOR_X_OFFSET,
        SELECTOR_Y,
        SELECTOR_WIDTH,
        SELECTOR_HEIGHT
    )){
        return
            AlgorithmSelectorAction::Toggle;
    }


    if(!dropdownOpen){

        return
            AlgorithmSelectorAction::None;
    }


    // =====================================
    // Dijkstra option
    // =====================================

    if(pointInsideRectangle(
        mousePosition,
        panelX + SELECTOR_X_OFFSET,
        SELECTOR_Y
            + SELECTOR_HEIGHT,
        SELECTOR_WIDTH,
        SELECTOR_HEIGHT
    )){
        return
            AlgorithmSelectorAction::
                SelectDijkstra;
    }


    // =====================================
    // A* option
    // =====================================

    if(pointInsideRectangle(
        mousePosition,
        panelX + SELECTOR_X_OFFSET,
        SELECTOR_Y
            + 2.0f * SELECTOR_HEIGHT,
        SELECTOR_WIDTH,
        SELECTOR_HEIGHT
    )){
        return
            AlgorithmSelectorAction::
                SelectAStar;
    }


    return
        AlgorithmSelectorAction::None;
}


void drawDemoPanel(
    sf::RenderWindow& window,
    const sf::Font& font,
    const DemoPanelData& data
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
        - DEMO_PANEL_WIDTH;


    // =====================================
    // Colors
    // =====================================

    const sf::Color panelColor(
        28,
        28,
        32
    );

    const sf::Color headingColor(
        255,
        255,
        255
    );

    const sf::Color secondaryColor(
        150,
        150,
        160
    );

    const sf::Color dividerColor(
        60,
        60,
        68
    );


    // =====================================
    // Panel background
    // =====================================

    sf::RectangleShape panelBackground(
        sf::Vector2f(
            DEMO_PANEL_WIDTH,
            windowHeight
        )
    );


    panelBackground.setPosition(
        sf::Vector2f(
            panelX,
            0.0f
        )
    );


    panelBackground.setFillColor(
        panelColor
    );


    window.draw(
        panelBackground
    );


    // =====================================
    // Title
    // =====================================

    drawPanelText(
        window,
        font,
        "MOTION PLANNER",
        panelX + 20.0f,
        18.0f,
        20,
        headingColor
    );


    drawPanelText(
        window,
        font,
        "Interactive Planning Sandbox",
        panelX + 20.0f,
        46.0f,
        11,
        secondaryColor
    );


    // =====================================
    // Divider
    // =====================================

    sf::RectangleShape divider(
        sf::Vector2f(
            DEMO_PANEL_WIDTH
                - 40.0f,
            1.0f
        )
    );


    divider.setPosition(
        sf::Vector2f(
            panelX + 20.0f,
            70.0f
        )
    );


    divider.setFillColor(
        dividerColor
    );


    window.draw(
        divider
    );


    // =====================================
    // Controls
    // =====================================

    drawPanelText(
        window,
        font,
        "CONTROLS",
        panelX + 20.0f,
        84.0f,
        11,
        secondaryColor
    );


    drawStatusRow(
        window,
        font,
        "Left Click",
        "Add Vertex",
        panelX,
        106.0f
    );


    drawStatusRow(
        window,
        font,
        "Enter",
        "Finish",
        panelX,
        127.0f
    );


    drawStatusRow(
        window,
        font,
        "Esc",
        "Cancel",
        panelX,
        148.0f
    );


    drawStatusRow(
        window,
        font,
        "S",
        "Set Start",
        panelX,
        169.0f
    );


    drawStatusRow(
        window,
        font,
        "G",
        "Set Goal",
        panelX,
        190.0f
    );


    drawStatusRow(
        window,
        font,
        "Space",
        "Plan",
        panelX,
        211.0f
    );


    drawStatusRow(
        window,
        font,
        "V",
        "Toggle Graph",
        panelX,
        232.0f
    );


    drawStatusRow(
        window,
        font,
        "R",
        "Reset",
        panelX,
        253.0f
    );


    // =====================================
    // Status
    // =====================================

    drawPanelText(
        window,
        font,
        "STATUS",
        panelX + 20.0f,
        286.0f,
        11,
        secondaryColor
    );


    drawStatusRow(
        window,
        font,
        "Mode",
        data.mode,
        panelX,
        308.0f
    );


    drawStatusRow(
        window,
        font,
        "Graph",
        data.graphStatus,
        panelX,
        329.0f
    );


    drawStatusRow(
        window,
        font,
        "Planner",
        data.plannerStatus,
        panelX,
        350.0f
    );


    // =====================================
    // Shortest path
    // =====================================

    drawPanelText(
        window,
        font,
        "SHORTEST PATH",
        panelX + 20.0f,
        383.0f,
        11,
        secondaryColor
    );


    if(!data.showAlgorithmSelector){

        drawStatusRow(
            window,
            font,
            "Algorithm",
            data.algorithm,
            panelX,
            405.0f
        );
    }


    std::string lengthText =
        "--";


    if(data.plannerStatus ==
       "Path Found"){

        lengthText =
            formatDouble(
                data.pathLength,
                2
            )
            + " px";
    }


    drawStatusRow(
        window,
        font,
        "Length",
        lengthText,
        panelX,
        data.showAlgorithmSelector
            ? 439.0f
            : 426.0f
    );


    std::string segmentText =
        "--";


    if(data.plannerStatus ==
       "Path Found"){

        segmentText =
            std::to_string(
                data.pathSegments
            );
    }


    drawStatusRow(
        window,
        font,
        "Segments",
        segmentText,
        panelX,
        data.showAlgorithmSelector
            ? 460.0f
            : 447.0f
    );


    if(data.showAlgorithmSelector){

        std::string expandedText =
            "--";

        if(data.plannerStatus !=
           "Not Run"){

            expandedText =
                std::to_string(
                    data.expandedNodes
                );
        }


        drawStatusRow(
            window,
            font,
            "Expanded",
            expandedText,
            panelX,
            481.0f
        );
    }


    // =====================================
    // Graph
    // =====================================

    const float graphHeadingY =
        data.showAlgorithmSelector
            ? 514.0f
            : 480.0f;


    drawPanelText(
        window,
        font,
        "GRAPH",
        panelX + 20.0f,
        graphHeadingY,
        11,
        secondaryColor
    );


    drawStatusRow(
        window,
        font,
        "Nodes",
        std::to_string(
            data.graphNodes
        ),
        panelX,
        graphHeadingY
            + 22.0f
    );


    drawStatusRow(
        window,
        font,
        "Edges",
        std::to_string(
            data.graphEdges
        ),
        panelX,
        graphHeadingY
            + 43.0f
    );


    drawStatusRow(
        window,
        font,
        "Obstacles",
        std::to_string(
            data.obstacleCount
        ),
        panelX,
        graphHeadingY
            + 64.0f
    );


    // =====================================
    // Performance
    // =====================================

    const float performanceHeadingY =
        data.showAlgorithmSelector
            ? 611.0f
            : 577.0f;


    drawPanelText(
        window,
        font,
        "PERFORMANCE",
        panelX + 20.0f,
        performanceHeadingY,
        11,
        secondaryColor
    );


    std::string graphBuildText =
        "--";

    std::string searchText =
        "--";

    std::string totalText =
        "--";


    if(data.plannerStatus !=
       "Not Run"){

        graphBuildText =
            formatDouble(
                data.graphBuildTimeMs,
                3
            )
            + " ms";

        searchText =
            formatDouble(
                data.searchTimeMs,
                3
            )
            + " ms";

        totalText =
            formatDouble(
                data.totalTimeMs,
                3
            )
            + " ms";
    }


    drawStatusRow(
        window,
        font,
        "Graph Build",
        graphBuildText,
        panelX,
        performanceHeadingY
            + 22.0f
    );


    drawStatusRow(
        window,
        font,
        "Search",
        searchText,
        panelX,
        performanceHeadingY
            + 43.0f
    );


    drawStatusRow(
        window,
        font,
        "Total",
        totalText,
        panelX,
        performanceHeadingY
            + 64.0f
    );


    // Draw the dropdown last so its options
    // appear above the other UI elements.
    if(data.showAlgorithmSelector){

        drawAlgorithmSelector(
            window,
            font,
            data,
            panelX
        );
    }
}