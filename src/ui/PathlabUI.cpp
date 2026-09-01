#include "PathlabUI.hpp"

#include <algorithm>
#include <cmath>
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

const sf::Color CONTROL_PRESSED{
    44,
    50,
    61
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

const sf::Color ACCENT_PRESSED{
    48,
    112,
    220
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

constexpr float CONTROL_RADIUS = 6.0f;
constexpr float COMPACT_CONTROL_RADIUS = 5.0f;
constexpr float CONTROL_HORIZONTAL_PADDING = 12.0f;

// =====================================
// Basic helpers
// =====================================

sf::ConvexShape makeRoundedRectangle(
    const sf::FloatRect& bounds,
    float requestedRadius
){
    constexpr std::size_t pointsPerCorner = 5;
    constexpr float pi = 3.14159265358979323846f;

    const float radius =
        std::min(
            requestedRadius,
            std::min(bounds.size.x, bounds.size.y) / 2.0f
        );

    sf::ConvexShape shape(pointsPerCorner * 4);

    const sf::Vector2f cornerCenters[4] = {
        {radius, radius},
        {bounds.size.x - radius, radius},
        {bounds.size.x - radius, bounds.size.y - radius},
        {radius, bounds.size.y - radius}
    };

    const float startAngles[4] = {
        pi,
        pi * 1.5f,
        0.0f,
        pi * 0.5f
    };

    std::size_t pointIndex = 0;

    for(std::size_t corner = 0; corner < 4; ++corner){
        for(std::size_t point = 0; point < pointsPerCorner; ++point){
            const float angle =
                startAngles[corner]
                + pi * 0.5f
                    * static_cast<float>(point)
                    / static_cast<float>(pointsPerCorner - 1);

            shape.setPoint(
                pointIndex,
                cornerCenters[corner]
                    + sf::Vector2f(
                        std::cos(angle) * radius,
                        std::sin(angle) * radius
                    )
            );

            ++pointIndex;
        }
    }

    shape.setPosition(bounds.position);

    return shape;
}

void drawRoundedSurface(
    sf::RenderWindow& window,
    const sf::FloatRect& bounds,
    float radius,
    const sf::Color& fillColor,
    const sf::Color& outlineColor,
    float outlineThickness = 1.0f
){
    sf::ConvexShape surface =
        makeRoundedRectangle(bounds, radius);

    surface.setFillColor(fillColor);
    surface.setOutlineThickness(outlineThickness);
    surface.setOutlineColor(outlineColor);

    window.draw(surface);
}

sf::Color getControlFill(
    bool hovered,
    bool pressed,
    bool active = false
){
    if(active){
        return pressed
            ? ACCENT_PRESSED
            : hovered
                ? ACCENT_HOVER
                : ACCENT;
    }

    if(pressed){
        return CONTROL_PRESSED;
    }

    return hovered
        ? CONTROL_HOVER
        : CONTROL;
}

void drawPlayIcon(
    sf::RenderWindow& window,
    const sf::Vector2f& center,
    const sf::Color& color
){
    sf::ConvexShape triangle(3);

    triangle.setPoint(0, center + sf::Vector2f(-3.5f, -5.0f));
    triangle.setPoint(1, center + sf::Vector2f(-3.5f, 5.0f));
    triangle.setPoint(2, center + sf::Vector2f(5.0f, 0.0f));
    triangle.setFillColor(color);

    window.draw(triangle);
}

void drawPauseIcon(
    sf::RenderWindow& window,
    const sf::Vector2f& center,
    const sf::Color& color
){
    sf::RectangleShape leftBar(sf::Vector2f(2.5f, 10.0f));
    sf::RectangleShape rightBar(sf::Vector2f(2.5f, 10.0f));

    leftBar.setPosition(center + sf::Vector2f(-4.0f, -5.0f));
    rightBar.setPosition(center + sf::Vector2f(1.5f, -5.0f));
    leftBar.setFillColor(color);
    rightBar.setFillColor(color);

    window.draw(leftBar);
    window.draw(rightBar);
}

void drawResetIcon(
    sf::RenderWindow& window,
    const sf::Vector2f& center,
    const sf::Color& color
){
    sf::RectangleShape resetBar(sf::Vector2f(2.0f, 11.0f));

    resetBar.setPosition(center + sf::Vector2f(-6.0f, -5.5f));
    resetBar.setFillColor(color);
    window.draw(resetBar);

    sf::ConvexShape triangle(3);

    triangle.setPoint(0, center + sf::Vector2f(-3.0f, 0.0f));
    triangle.setPoint(1, center + sf::Vector2f(5.0f, -5.0f));
    triangle.setPoint(2, center + sf::Vector2f(5.0f, 5.0f));
    triangle.setFillColor(color);

    window.draw(triangle);
}

void drawStepIcon(
    sf::RenderWindow& window,
    const sf::Vector2f& center,
    const sf::Color& color
){
    drawPlayIcon(
        window,
        center + sf::Vector2f(-1.5f, 0.0f),
        color
    );

    sf::RectangleShape stepBar(sf::Vector2f(2.0f, 11.0f));

    stepBar.setPosition(center + sf::Vector2f(5.0f, -5.5f));
    stepBar.setFillColor(color);
    window.draw(stepBar);
}

void drawChevron(
    sf::RenderWindow& window,
    const sf::Vector2f& center,
    bool pointsUp,
    const sf::Color& color
){
    const float direction = pointsUp ? -1.0f : 1.0f;

    sf::VertexArray chevron(sf::PrimitiveType::LineStrip);

    chevron.append(
        sf::Vertex{
            center + sf::Vector2f(-4.0f, -2.0f * direction),
            color
        }
    );
    chevron.append(
        sf::Vertex{
            center + sf::Vector2f(0.0f, 2.0f * direction),
            color
        }
    );
    chevron.append(
        sf::Vertex{
            center + sf::Vector2f(4.0f, -2.0f * direction),
            color
        }
    );

    window.draw(chevron);
}

void drawHorizontalChevron(
    sf::RenderWindow& window,
    const sf::Vector2f& center,
    bool pointsRight,
    const sf::Color& color
){
    const float direction = pointsRight ? 1.0f : -1.0f;

    sf::VertexArray chevron(sf::PrimitiveType::LineStrip);
    chevron.append(
        sf::Vertex{
            center + sf::Vector2f(-2.0f * direction, -4.0f),
            color
        }
    );
    chevron.append(
        sf::Vertex{
            center + sf::Vector2f(2.0f * direction, 0.0f),
            color
        }
    );
    chevron.append(
        sf::Vertex{
            center + sf::Vector2f(-2.0f * direction, 4.0f),
            color
        }
    );

    window.draw(chevron);
}

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

// =====================================
// Checkbox-style visualization row
// =====================================

void drawCheckboxRow(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& label,
    bool checked,
    bool enabled,
    float panelX,
    float y,
    const sf::Color& previewColor
){
    const sf::FloatRect rowBounds(
        sf::Vector2f(panelX + 14.0f, y - 3.0f),
        sf::Vector2f(PATHLAB_SIDE_PANEL_WIDTH - 28.0f, 21.0f)
    );

    const sf::Vector2i mousePosition =
        sf::Mouse::getPosition(window);

    const bool hovered =
        enabled
        &&
        containsPoint(rowBounds, mousePosition);

    const bool visiblyChecked = enabled && checked;

    if(hovered){
        drawRoundedSurface(
            window,
            rowBounds,
            COMPACT_CONTROL_RADIUS,
            sf::Color(255, 255, 255, 8),
            sf::Color::Transparent,
            0.0f
        );
    }

    const float checkboxX =
        panelX + 20.0f;

    const float checkboxY =
        y + 1.0f;


    const sf::FloatRect checkboxBounds(
        sf::Vector2f(checkboxX, checkboxY),
        sf::Vector2f(14.0f, 14.0f)
    );

    drawRoundedSurface(
        window,
        checkboxBounds,
        3.0f,
        !enabled
            ? sf::Color(27, 30, 36)
            : visiblyChecked
            ? ACCENT
            : hovered
                ? CONTROL_HOVER
                : CONTROL,
        !enabled
            ? sf::Color(42, 46, 55)
            : visiblyChecked
            ? ACCENT
            : hovered
                ? sf::Color(78, 86, 101)
                : BORDER
    );


    if(visiblyChecked){
        sf::VertexArray checkmark(sf::PrimitiveType::LineStrip);

        checkmark.append(
            sf::Vertex{
                sf::Vector2f(checkboxX + 3.0f, checkboxY + 7.0f),
                sf::Color::White
            }
        );
        checkmark.append(
            sf::Vertex{
                sf::Vector2f(checkboxX + 6.0f, checkboxY + 10.0f),
                sf::Color::White
            }
        );
        checkmark.append(
            sf::Vertex{
                sf::Vector2f(checkboxX + 11.0f, checkboxY + 4.0f),
                sf::Color::White
            }
        );

        window.draw(checkmark);
    }


    drawText(
        window,
        font,
        label,
        panelX + 44.0f,
        y,
        12,
        !enabled
            ? TEXT_MUTED
            : hovered
                ? TEXT_PRIMARY
                : TEXT_SECONDARY
    );


    sf::CircleShape preview(
        4.0f
    );

    preview.setFillColor(
        enabled
            ? previewColor
            : sf::Color(74, 79, 89)
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

sf::Color getPlannerStatusColor(const std::string& status){
    if(status == "Ready" || status == "Path Found"){
        return sf::Color(82, 181, 119);
    }

    if(status == "No Path"){
        return sf::Color(220, 116, 101);
    }

    return TEXT_MUTED;
}

void drawStatusBadge(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& status,
    float panelX,
    float y
){
    const sf::Color statusColor = getPlannerStatusColor(status);


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

    const bool pressed =
        hovered
        &&
        sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    drawRoundedSurface(
        window,
        bounds,
        CONTROL_RADIUS,
        getControlFill(hovered, pressed),
        dropdownOpen
            ? ACCENT
            : BORDER
    );


    drawText(
        window,
        font,
        algorithm,
        bounds.position.x + CONTROL_HORIZONTAL_PADDING,
        bounds.position.y + 7.0f,
        12,
        TEXT_PRIMARY
    );

    drawChevron(
        window,
        sf::Vector2f(
            bounds.position.x + bounds.size.x - 16.0f,
            bounds.position.y + bounds.size.y / 2.0f
        ),
        dropdownOpen,
        hovered
            ? TEXT_PRIMARY
            : TEXT_SECONDARY
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

    const sf::FloatRect firstOptionBounds =
        getAlgorithmOptionBounds(window.getSize(), 0);

    const sf::FloatRect menuBounds(
        firstOptionBounds.position,
        sf::Vector2f(
            firstOptionBounds.size.x,
            firstOptionBounds.size.y * 2.0f
        )
    );

    drawRoundedSurface(
        window,
        menuBounds,
        CONTROL_RADIUS,
        sf::Color(29, 33, 41, 250),
        BORDER
    );


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

        const bool selected =
            options[i] == selectedAlgorithm;

        if(hovered || selected){
            const sf::FloatRect highlightBounds(
                bounds.position + sf::Vector2f(4.0f, 3.0f),
                bounds.size - sf::Vector2f(8.0f, 6.0f)
            );

            drawRoundedSurface(
                window,
                highlightBounds,
                COMPACT_CONTROL_RADIUS,
                hovered
                    ? CONTROL_HOVER
                    : sf::Color(ACCENT.r, ACCENT.g, ACCENT.b, 24),
                sf::Color::Transparent,
                0.0f
            );
        }


        const sf::Color textColor =
            selected
                ? ACCENT
                : TEXT_PRIMARY;


        drawText(
            window,
            font,
            options[i],
            bounds.position.x + CONTROL_HORIZONTAL_PADDING,
            bounds.position.y + 7.0f,
            12,
            textColor
        );

        if(selected){
            sf::CircleShape selectedIndicator(3.0f);

            selectedIndicator.setFillColor(ACCENT);
            selectedIndicator.setPosition(
                sf::Vector2f(
                    bounds.position.x + bounds.size.x - 18.0f,
                    bounds.position.y + 13.0f
                )
            );

            window.draw(selectedIndicator);
        }

        if(i == 0){
            drawDivider(
                window,
                bounds.position.x + 8.0f,
                bounds.position.y + bounds.size.y,
                bounds.size.x - 16.0f
            );
        }
    }
}

// =====================================
// Playback Dock
// =====================================

enum class PlaybackControl{
    Reset,
    Toggle,
    Step,
    Speed
};

void drawPlaybackButton(
    sf::RenderWindow& window,
    const sf::Font& font,
    const sf::FloatRect& bounds,
    PlaybackControl control,
    const std::string& speedLabel,
    bool hovered,
    bool pressed,
    bool playbackPlaying
){
    const bool active =
        control == PlaybackControl::Toggle
        &&
        playbackPlaying;

    drawRoundedSurface(
        window,
        bounds,
        8.0f,
        getControlFill(hovered, pressed, active),
        active
            ? sf::Color(ACCENT_HOVER.r, ACCENT_HOVER.g, ACCENT_HOVER.b, 180)
            : sf::Color(BORDER.r, BORDER.g, BORDER.b, 180)
    );

    const sf::Color iconColor =
        active
            ? sf::Color::White
            : control == PlaybackControl::Speed
                ? ACCENT_HOVER
                : hovered
                    ? TEXT_PRIMARY
                    : TEXT_SECONDARY;

    const sf::Vector2f center =
        bounds.position + bounds.size / 2.0f;

    switch(control){
        case PlaybackControl::Reset:
            drawResetIcon(window, center, iconColor);
            break;

        case PlaybackControl::Toggle:
            if(playbackPlaying){
                drawPauseIcon(window, center, iconColor);
            } else {
                drawPlayIcon(window, center, iconColor);
            }
            break;

        case PlaybackControl::Step:
            drawStepIcon(window, center, iconColor);
            break;

        case PlaybackControl::Speed:{
            sf::Text speedText(font);

            speedText.setString(speedLabel);
            speedText.setCharacterSize(11);
            speedText.setFillColor(iconColor);

            const sf::FloatRect textBounds =
                speedText.getLocalBounds();

            speedText.setPosition(
                sf::Vector2f(
                    center.x
                        - textBounds.size.x / 2.0f
                        - textBounds.position.x,
                    center.y
                        - textBounds.size.y / 2.0f
                        - textBounds.position.y
                        - 1.0f
                )
            );

            window.draw(speedText);
            break;
        }
    }
}

void drawPlaybackDock(
    sf::RenderWindow& window,
    const sf::Font& font,
    const PathlabUIData& data
){
    const sf::FloatRect dock =
        getPlaybackDockBounds(
            window.getSize(),
            data.sidebarVisible
        );


    const sf::FloatRect shadowBounds(
        dock.position + sf::Vector2f(0.0f, 5.0f),
        dock.size
    );

    drawRoundedSurface(
        window,
        shadowBounds,
        12.0f,
        sf::Color(4, 6, 10, 105),
        sf::Color::Transparent,
        0.0f
    );

    drawRoundedSurface(
        window,
        dock,
        12.0f,
        sf::Color(24, 28, 35, 235),
        sf::Color(83, 91, 105, 175)
    );


    const sf::Vector2i mousePosition =
        sf::Mouse::getPosition(window);

    const sf::FloatRect resetBounds =
        getPlaybackResetBounds(window.getSize(), data.sidebarVisible);

    const sf::FloatRect toggleBounds =
        getPlaybackToggleBounds(window.getSize(), data.sidebarVisible);

    const sf::FloatRect stepBounds =
        getPlaybackStepBounds(window.getSize(), data.sidebarVisible);

    const sf::FloatRect speedBounds =
        getPlaybackSpeedBounds(window.getSize(), data.sidebarVisible);

    const bool leftPressed =
        sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    const bool resetHovered =
        containsPoint(resetBounds, mousePosition);

    const bool toggleHovered =
        containsPoint(toggleBounds, mousePosition);

    const bool stepHovered =
        containsPoint(stepBounds, mousePosition);

    const bool speedHovered =
        containsPoint(speedBounds, mousePosition);


    drawPlaybackButton(
        window,
        font,
        resetBounds,
        PlaybackControl::Reset,
        "",
        resetHovered,
        resetHovered && leftPressed,
        data.playbackPlaying
    );

    drawPlaybackButton(
        window,
        font,
        toggleBounds,
        PlaybackControl::Toggle,
        "",
        toggleHovered,
        toggleHovered && leftPressed,
        data.playbackPlaying
    );

    drawPlaybackButton(
        window,
        font,
        stepBounds,
        PlaybackControl::Step,
        "",
        stepHovered,
        stepHovered && leftPressed,
        data.playbackPlaying
    );

    drawPlaybackButton(
        window,
        font,
        speedBounds,
        PlaybackControl::Speed,
        data.playbackSpeed,
        speedHovered,
        speedHovered && leftPressed,
        data.playbackPlaying
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
        dock.position.x + dock.size.x - 111.0f,
        dock.position.y + 20.0f,
        9,
        TEXT_MUTED
    );


    const float progressRatio =
        data.playbackTotal > 0
            ? std::min(
                1.0f,
                static_cast<float>(
                    data.playbackIndex
                )
                / static_cast<float>(
                    data.playbackTotal
                )
            )
            : 0.0f;


    const float barX =
        dock.position.x + 14.0f;

    const float barY =
        dock.position.y + 58.0f;

    const float barWidth =
        dock.size.x - 92.0f;

    constexpr float barHeight =
        3.0f;


    const sf::FloatRect progressBounds(
        sf::Vector2f(barX, barY),
        sf::Vector2f(barWidth, barHeight)
    );

    drawRoundedSurface(
        window,
        progressBounds,
        barHeight / 2.0f,
        BORDER,
        sf::Color::Transparent,
        0.0f
    );

    if(progressRatio > 0.0f){
        const sf::FloatRect progressFillBounds(
            sf::Vector2f(barX, barY),
            sf::Vector2f(barWidth * progressRatio, barHeight)
        );

        drawRoundedSurface(
            window,
            progressFillBounds,
            barHeight / 2.0f,
            ACCENT,
            sf::Color::Transparent,
            0.0f
        );
    }


    sf::Text progressText(font);

    progressText.setString(progress);
    progressText.setCharacterSize(10);
    progressText.setFillColor(TEXT_PRIMARY);

    const sf::FloatRect progressTextBounds =
        progressText.getLocalBounds();

    progressText.setPosition(
        sf::Vector2f(
            dock.position.x
                + dock.size.x
                - 14.0f
                - progressTextBounds.size.x,
            dock.position.y + 50.0f
        )
    );

    window.draw(progressText);
}

// =====================================
// Run planner button
// =====================================

void drawRunPlannerButton(
    sf::RenderWindow& window,
    const sf::Font& font,
    bool enabled
){
    const sf::FloatRect bounds = getRunPlannerButtonBounds(window.getSize());

    const sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

    const bool hovered = enabled && containsPoint(bounds, mousePosition);

    const bool pressed =
        enabled
        &&
        hovered
        &&
        sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    drawRoundedSurface(
        window,
        bounds,
        8.0f,
        enabled
            ? getControlFill(hovered, pressed, true)
            : sf::Color(31, 35, 43),
        enabled
            ? hovered
                ? sf::Color(ACCENT_HOVER.r, ACCENT_HOVER.g, ACCENT_HOVER.b, 210)
                : sf::Color(ACCENT_HOVER.r, ACCENT_HOVER.g, ACCENT_HOVER.b, 120)
            : BORDER
    );


    sf::Text buttonText(font);

    buttonText.setString("RUN PLANNER");
    buttonText.setCharacterSize(12);
    const sf::Color contentColor =
        enabled ? sf::Color::White : TEXT_MUTED;

    buttonText.setFillColor(contentColor);

    const sf::FloatRect textBounds =
        buttonText.getLocalBounds();

    constexpr float iconWidth = 10.0f;
    constexpr float iconGap = 10.0f;

    const float contentWidth =
        iconWidth + iconGap + textBounds.size.x;

    const float contentX =
        bounds.position.x
        + (bounds.size.x - contentWidth) / 2.0f;

    drawPlayIcon(
        window,
        sf::Vector2f(
            contentX + iconWidth / 2.0f,
            bounds.position.y + bounds.size.y / 2.0f
        ),
        contentColor
    );

    buttonText.setPosition(
        sf::Vector2f(
            contentX
                + iconWidth
                + iconGap
                - textBounds.position.x,
            bounds.position.y
                + (bounds.size.y - textBounds.size.y) / 2.0f
                - textBounds.position.y
                - 1.0f
        )
    );

    window.draw(buttonText);
}

// =====================================
// Top bar
// =====================================

void drawTopBar(
    sf::RenderWindow& window,
    const sf::Font& font,
    const PathlabUIData& data
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
    // Planner status
    // ---------------------------------

    const sf::Color statusColor =
        getPlannerStatusColor(data.plannerStatus);

    sf::Text statusText(font);
    statusText.setString(data.plannerStatus);
    statusText.setCharacterSize(11);
    statusText.setFillColor(statusColor);

    const sf::FloatRect statusTextBounds = statusText.getLocalBounds();
    const sf::FloatRect sidebarToggleBounds =
        getSidebarToggleButtonBounds(window.getSize());

    const float statusRight = sidebarToggleBounds.position.x - 16.0f;
    const float statusTextX =
        statusRight - statusTextBounds.size.x;

    statusText.setPosition(sf::Vector2f(statusTextX, 22.0f));
    window.draw(statusText);

    sf::CircleShape statusIndicator(4.0f);
    statusIndicator.setFillColor(statusColor);
    statusIndicator.setPosition(sf::Vector2f(statusTextX - 15.0f, 29.0f));
    window.draw(statusIndicator);


    // ---------------------------------
    // Help button
    // ---------------------------------

    const sf::FloatRect helpButtonBounds =
        getHelpButtonBounds(window.getSize());

    const sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    const bool helpButtonHovered =
        containsPoint(helpButtonBounds, mousePosition);
    const bool helpButtonPressed =
        helpButtonHovered
        && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    drawRoundedSurface(
        window,
        helpButtonBounds,
        CONTROL_RADIUS,
        getControlFill(helpButtonHovered, helpButtonPressed),
        helpButtonHovered ? sf::Color(78, 86, 101) : BORDER
    );

    sf::Text helpButtonText(font);
    helpButtonText.setString("?");
    helpButtonText.setCharacterSize(16);
    helpButtonText.setFillColor(
        helpButtonHovered ? TEXT_PRIMARY : TEXT_SECONDARY
    );

    const sf::FloatRect helpTextBounds = helpButtonText.getLocalBounds();
    helpButtonText.setPosition(
        sf::Vector2f(
            helpButtonBounds.position.x
                + (helpButtonBounds.size.x - helpTextBounds.size.x) / 2.0f
                - helpTextBounds.position.x,
            helpButtonBounds.position.y
                + (helpButtonBounds.size.y - helpTextBounds.size.y) / 2.0f
                - helpTextBounds.position.y
                - 1.0f
        )
    );
    window.draw(helpButtonText);


    drawDivider(
        window,
        0.0f,
        PATHLAB_TOP_BAR_HEIGHT
            - 1.0f,
        windowWidth
    );
}

void drawSidebarToggleButton(
    sf::RenderWindow& window,
    bool sidebarVisible
){
    const sf::FloatRect bounds =
        getSidebarToggleButtonBounds(window.getSize());

    const sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    const bool hovered = containsPoint(bounds, mousePosition);
    const bool pressed =
        hovered
        && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    drawRoundedSurface(
        window,
        bounds,
        CONTROL_RADIUS,
        getControlFill(hovered, pressed),
        hovered ? sf::Color(78, 86, 101) : BORDER
    );

    const sf::Color iconColor = hovered ? TEXT_PRIMARY : TEXT_SECONDARY;
    const sf::Vector2f framePosition =
        bounds.position + sf::Vector2f(8.0f, 9.0f);

    sf::RectangleShape frame(sf::Vector2f(18.0f, 16.0f));
    frame.setPosition(framePosition);
    frame.setFillColor(sf::Color::Transparent);
    frame.setOutlineThickness(1.0f);
    frame.setOutlineColor(iconColor);
    window.draw(frame);

    sf::RectangleShape divider(sf::Vector2f(1.0f, 14.0f));
    divider.setPosition(framePosition + sf::Vector2f(10.0f, 1.0f));
    divider.setFillColor(iconColor);
    window.draw(divider);

    drawHorizontalChevron(
        window,
        framePosition + sf::Vector2f(14.0f, 8.0f),
        sidebarVisible,
        iconColor
    );
}

void drawResetCameraButton(
    sf::RenderWindow& window,
    const sf::Font& font,
    bool sidebarVisible
){
    const sf::FloatRect bounds =
        getResetCameraButtonBounds(window.getSize(), sidebarVisible);

    const sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    const bool hovered = containsPoint(bounds, mousePosition);
    const bool pressed =
        hovered
        && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    drawRoundedSurface(
        window,
        bounds,
        CONTROL_RADIUS,
        getControlFill(hovered, pressed),
        hovered ? sf::Color(78, 86, 101) : BORDER
    );

    sf::RectangleShape viewFrame(sf::Vector2f(13.0f, 10.0f));
    viewFrame.setPosition(
        sf::Vector2f(
            bounds.position.x + 12.0f,
            bounds.position.y + 11.0f
        )
    );
    viewFrame.setFillColor(sf::Color::Transparent);
    viewFrame.setOutlineThickness(1.0f);
    viewFrame.setOutlineColor(hovered ? ACCENT_HOVER : TEXT_SECONDARY);
    window.draw(viewFrame);

    sf::CircleShape viewCenter(2.0f);
    viewCenter.setPosition(
        sf::Vector2f(
            bounds.position.x + 16.5f,
            bounds.position.y + 14.0f
        )
    );
    viewCenter.setFillColor(hovered ? ACCENT_HOVER : TEXT_SECONDARY);
    window.draw(viewCenter);

    drawText(
        window,
        font,
        "RESET VIEW",
        bounds.position.x + 34.0f,
        bounds.position.y + 8.0f,
        10,
        hovered ? TEXT_PRIMARY : TEXT_SECONDARY
    );
}

void drawLoadDemoSceneButton(
    sf::RenderWindow& window,
    const sf::Font& font,
    bool sidebarVisible
){
    const sf::FloatRect bounds =
        getLoadDemoSceneButtonBounds(window.getSize(), sidebarVisible);

    const sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    const bool hovered = containsPoint(bounds, mousePosition);
    const bool pressed =
        hovered
        && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    drawRoundedSurface(
        window,
        bounds,
        CONTROL_RADIUS,
        getControlFill(hovered, pressed),
        hovered ? sf::Color(78, 86, 101) : BORDER
    );

    const sf::Color iconColor = hovered ? ACCENT_HOVER : TEXT_SECONDARY;

    sf::RectangleShape firstObstacle(sf::Vector2f(5.0f, 10.0f));
    firstObstacle.setPosition(
        sf::Vector2f(bounds.position.x + 11.0f, bounds.position.y + 11.0f)
    );
    firstObstacle.setFillColor(iconColor);
    window.draw(firstObstacle);

    sf::RectangleShape secondObstacle(sf::Vector2f(5.0f, 7.0f));
    secondObstacle.setPosition(
        sf::Vector2f(bounds.position.x + 18.0f, bounds.position.y + 14.0f)
    );
    secondObstacle.setFillColor(iconColor);
    window.draw(secondObstacle);

    drawText(
        window,
        font,
        "LOAD DEMO",
        bounds.position.x + 34.0f,
        bounds.position.y + 8.0f,
        10,
        hovered ? TEXT_PRIMARY : TEXT_SECONDARY
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

    const bool compact =
        useCompactSidebarLayout(windowSize);

    const float initialTopPadding =
        compact ? 8.0f : 16.0f;

    const float plannerLabelGap =
        compact ? 18.0f : 21.0f;

    const float plannerBlockHeight =
        compact ? 52.0f : 69.0f;

    const float sectionTopGap =
        compact ? 8.0f : 14.0f;

    const float sectionHeadingGap =
        compact ? 19.0f : 24.0f;

    const float visualizationRowSpacing =
        compact ? 19.0f : 23.0f;

    const float valueRowSpacing =
        compact ? 18.0f : 21.0f;

    const float statusToValueGap =
        compact ? 21.0f : 24.0f;

    const float sectionBottomGap =
        compact ? 22.0f : 27.0f;


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
        + initialTopPadding;


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


    y += plannerLabelGap;


    drawAlgorithmSelector(
        window,
        font,
        data.algorithm,
        data.algorithmDropdownOpen,
        panelX,
        y
    );


    y += plannerBlockHeight;


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

    y += sectionTopGap;


    drawSectionHeading(
        window,
        font,
        "VISUALIZATION",
        panelX + 20.0f,
        y
    );


    y += sectionHeadingGap;


    drawCheckboxRow(
        window,
        font,
        "Obstacles",
        data.showObstacles,
        data.hasObstacles,
        panelX,
        y,
        sf::Color(
            170,
            175,
            185
        )
    );


    y += visualizationRowSpacing;


    drawCheckboxRow(
        window,
        font,
        "Visibility Graph",
        data.showVisibilityGraph,
        data.hasVisibilityGraph,
        panelX,
        y,
        sf::Color(
            105,
            110,
            120
        )
    );


    y += visualizationRowSpacing;


    drawCheckboxRow(
        window,
        font,
        "Final Path",
        data.showFinalPath,
        data.hasFinalPath,
        panelX,
        y,
        WARNING
    );

    if(data.hasSearchTrace){

        y += visualizationRowSpacing;

        drawCheckboxRow(
            window,
            font,
            "Explored Nodes",
            data.showExploredNodes,
            true,
            panelX,
            y,
            sf::Color(
                59,
                130,
                246
            )
        );
    }

    y += sectionBottomGap;

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

    y += sectionTopGap;

    drawSectionHeading(
        window,
        font,
        "SCENE",
        panelX + 20.0f,
        y
    );

    y += sectionHeadingGap;

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

    y += valueRowSpacing;

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


    y += valueRowSpacing;


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


    y += sectionBottomGap;


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

    y += sectionTopGap;


    drawSectionHeading(
        window,
        font,
        "RESULTS",
        panelX + 20.0f,
        y
    );


    y += sectionHeadingGap;


    drawStatusBadge(
        window,
        font,
        data.plannerStatus,
        panelX,
        y
    );


    y += statusToValueGap;


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


    y += valueRowSpacing;


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


    y += valueRowSpacing;


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

    y += valueRowSpacing;


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


    y += sectionBottomGap;


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

    y += sectionTopGap;


    drawSectionHeading(
        window,
        font,
        "PERFORMANCE",
        panelX + 20.0f,
        y
    );


    y += sectionHeadingGap;


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


    y += valueRowSpacing;


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


    y += valueRowSpacing;


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
        font,
        data.canRunPlanner
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


    const sf::FloatRect chipBounds(
        sf::Vector2f(x, y),
        sf::Vector2f(chipWidth, 20.0f)
    );

    drawRoundedSurface(
        window,
        chipBounds,
        4.0f,
        CONTROL,
        BORDER
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


    drawShortcut(
        window,
        font,
        "OPT+LMB",
        "Pan",
        665.0f,
        shortcutY
    );


    drawShortcut(
        window,
        font,
        "SCROLL",
        "Zoom",
        770.0f,
        shortcutY
    );
}

// =====================================
// Help overlay
// =====================================

void drawHelpControlRow(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& control,
    const std::string& description,
    float x,
    float y
){
    constexpr float chipWidth = 104.0f;
    constexpr float chipHeight = 21.0f;

    drawRoundedSurface(
        window,
        sf::FloatRect(
            sf::Vector2f(x, y),
            sf::Vector2f(chipWidth, chipHeight)
        ),
        4.0f,
        CONTROL,
        BORDER
    );

    drawText(
        window,
        font,
        control,
        x + 8.0f,
        y + 3.0f,
        9,
        TEXT_PRIMARY
    );

    drawText(
        window,
        font,
        description,
        x + chipWidth + 10.0f,
        y + 2.0f,
        11,
        TEXT_SECONDARY
    );
}

void drawHelpSectionHeading(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& heading,
    float x,
    float y,
    float width
){
    drawText(window, font, heading, x, y, 10, ACCENT_HOVER);
    drawDivider(window, x, y + 18.0f, width);
}

void drawHelpLegendItem(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& label,
    const sf::Color& color,
    float x,
    float y
){
    sf::CircleShape marker(4.0f);
    marker.setPosition(sf::Vector2f(x, y + 5.0f));
    marker.setFillColor(color);
    window.draw(marker);

    drawText(
        window,
        font,
        label,
        x + 16.0f,
        y,
        11,
        TEXT_SECONDARY
    );
}

void drawHelpCloseButton(
    sf::RenderWindow& window
){
    const sf::FloatRect bounds =
        getHelpCloseButtonBounds(window.getSize());

    const sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    const bool hovered = containsPoint(bounds, mousePosition);
    const bool pressed =
        hovered
        && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    drawRoundedSurface(
        window,
        bounds,
        CONTROL_RADIUS,
        getControlFill(hovered, pressed),
        hovered ? sf::Color(78, 86, 101) : BORDER
    );

    const sf::Vector2f center = bounds.position + bounds.size / 2.0f;
    const sf::Color iconColor = hovered ? TEXT_PRIMARY : TEXT_SECONDARY;

    sf::VertexArray closeIcon(sf::PrimitiveType::Lines);
    closeIcon.append(sf::Vertex{center + sf::Vector2f(-4.5f, -4.5f), iconColor});
    closeIcon.append(sf::Vertex{center + sf::Vector2f(4.5f, 4.5f), iconColor});
    closeIcon.append(sf::Vertex{center + sf::Vector2f(4.5f, -4.5f), iconColor});
    closeIcon.append(sf::Vertex{center + sf::Vector2f(-4.5f, 4.5f), iconColor});
    window.draw(closeIcon);
}

void drawHelpOverlay(
    sf::RenderWindow& window,
    const sf::Font& font
){
    const sf::Vector2u windowSize = window.getSize();
    const sf::FloatRect panelBounds = getHelpPanelBounds(windowSize);

    sf::RectangleShape backdrop(
        sf::Vector2f(
            static_cast<float>(windowSize.x),
            static_cast<float>(windowSize.y)
        )
    );
    backdrop.setFillColor(sf::Color(5, 7, 11, 205));
    window.draw(backdrop);

    drawRoundedSurface(
        window,
        sf::FloatRect(
            panelBounds.position + sf::Vector2f(0.0f, 6.0f),
            panelBounds.size
        ),
        12.0f,
        sf::Color(2, 4, 8, 120),
        sf::Color::Transparent,
        0.0f
    );

    drawRoundedSurface(
        window,
        panelBounds,
        12.0f,
        sf::Color(24, 27, 34, 252),
        sf::Color(69, 76, 89)
    );

    const float contentLeft = panelBounds.position.x + 28.0f;
    const float contentRight = panelBounds.position.x + panelBounds.size.x - 28.0f;
    const float columnGap = 34.0f;
    const float columnWidth =
        (contentRight - contentLeft - columnGap) / 2.0f;
    const float rightColumnX = contentLeft + columnWidth + columnGap;

    drawText(
        window,
        font,
        "HELP / CONTROLS",
        contentLeft,
        panelBounds.position.y + 18.0f,
        18,
        TEXT_PRIMARY
    );

    drawText(
        window,
        font,
        "Build a scene, run a planner, and inspect its search.",
        contentLeft,
        panelBounds.position.y + 43.0f,
        11,
        TEXT_MUTED
    );

    drawText(
        window,
        font,
        "? or Esc to close",
        panelBounds.position.x + panelBounds.size.x - 166.0f,
        panelBounds.position.y + 53.0f,
        10,
        TEXT_MUTED
    );

    drawHelpCloseButton(window);

    const float sectionStartY = panelBounds.position.y + 79.0f;
    constexpr float rowSpacing = 25.0f;
    constexpr float sectionGap = 15.0f;

    float leftY = sectionStartY;

    drawHelpSectionHeading(
        window, font, "SCENE EDITING", contentLeft, leftY, columnWidth
    );
    leftY += 26.0f;

    drawHelpControlRow(window, font, "LEFT CLICK", "Add obstacle vertex", contentLeft, leftY);
    leftY += rowSpacing;
    drawHelpControlRow(window, font, "ENTER", "Finalize polygon (3+ vertices)", contentLeft, leftY);
    leftY += rowSpacing;
    drawHelpControlRow(window, font, "S + CLICK", "Set or replace start", contentLeft, leftY);
    leftY += rowSpacing;
    drawHelpControlRow(window, font, "G + CLICK", "Set or replace goal", contentLeft, leftY);
    leftY += rowSpacing;
    drawHelpControlRow(window, font, "ESC", "Cancel mode or discard draft", contentLeft, leftY);
    leftY += rowSpacing;
    drawHelpControlRow(window, font, "R", "Clear scene and planner result", contentLeft, leftY);
    leftY += rowSpacing;
    drawHelpControlRow(window, font, "LOAD DEMO", "Load showcase and reset view", contentLeft, leftY);
    leftY += rowSpacing + sectionGap;

    drawHelpSectionHeading(
        window, font, "PLANNER", contentLeft, leftY, columnWidth
    );
    leftY += 26.0f;

    drawHelpControlRow(window, font, "SPACE", "Run the selected planner", contentLeft, leftY);
    leftY += rowSpacing;
    drawHelpControlRow(window, font, "ALGORITHM", "Choose Dijkstra or A*", contentLeft, leftY);
    leftY += rowSpacing + sectionGap;

    drawHelpSectionHeading(
        window, font, "CAMERA", contentLeft, leftY, columnWidth
    );
    leftY += 26.0f;

    drawHelpControlRow(window, font, "SCROLL", "Zoom around the pointer", contentLeft, leftY);
    leftY += rowSpacing;
    drawHelpControlRow(window, font, "OPT + DRAG", "Pan the canvas", contentLeft, leftY);
    leftY += rowSpacing;
    drawHelpControlRow(window, font, "RESET VIEW", "Restore opening camera", contentLeft, leftY);
    leftY += rowSpacing;
    drawHelpControlRow(window, font, "TAB", "Show or hide planner sidebar", contentLeft, leftY);

    float rightY = sectionStartY;

    drawHelpSectionHeading(
        window, font, "VISUALIZATION", rightColumnX, rightY, columnWidth
    );
    rightY += 26.0f;

    drawHelpControlRow(window, font, "V", "Toggle visibility graph", rightColumnX, rightY);
    rightY += rowSpacing;
    drawHelpControlRow(window, font, "SIDEBAR", "Toggle obstacles or final path", rightColumnX, rightY);
    rightY += rowSpacing;
    drawHelpControlRow(window, font, "SIDEBAR", "Toggle explored nodes after run", rightColumnX, rightY);
    rightY += rowSpacing + sectionGap;

    drawHelpSectionHeading(
        window, font, "SEARCH PLAYBACK", rightColumnX, rightY, columnWidth
    );
    rightY += 26.0f;

    drawHelpControlRow(window, font, "RESET", "Return trace to the beginning", rightColumnX, rightY);
    rightY += rowSpacing;
    drawHelpControlRow(window, font, "PLAY / PAUSE", "Animate or pause expansions", rightColumnX, rightY);
    rightY += rowSpacing;
    drawHelpControlRow(window, font, "STEP", "Advance one expansion", rightColumnX, rightY);
    rightY += rowSpacing;
    drawHelpControlRow(window, font, "SPEED", "Cycle 0.25x through 4x", rightColumnX, rightY);
    rightY += rowSpacing + sectionGap;

    drawHelpSectionHeading(
        window, font, "VISUAL LEGEND", rightColumnX, rightY, columnWidth
    );
    rightY += 27.0f;

    const float legendSecondColumnX = rightColumnX + columnWidth * 0.52f;

    drawHelpLegendItem(window, font, "Start", SUCCESS, rightColumnX, rightY);
    drawHelpLegendItem(window, font, "Goal", DANGER, legendSecondColumnX, rightY);
    rightY += 23.0f;
    drawHelpLegendItem(window, font, "Final path", WARNING, rightColumnX, rightY);
    drawHelpLegendItem(window, font, "Explored", ACCENT, legendSecondColumnX, rightY);
    rightY += 23.0f;
    drawHelpLegendItem(
        window,
        font,
        "Current expansion",
        sf::Color(34, 211, 238),
        rightColumnX,
        rightY
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
    drawTopBar(window, font, data);

    if(data.sidebarVisible){
        drawSidePanel(window, font, data);
    }

    drawBottomBar(window, font);

    drawResetCameraButton(window, font, data.sidebarVisible);

    drawLoadDemoSceneButton(window, font, data.sidebarVisible);

    drawSidebarToggleButton(window, data.sidebarVisible);

    if(data.sidebarVisible && data.algorithmDropdownOpen){

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

    if(data.helpOverlayOpen){
        drawHelpOverlay(window, font);
    }
}

PathlabUIAction handlePathlabUIClick(
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
