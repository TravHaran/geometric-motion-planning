#pragma once
#include "PathlabUI.hpp"
#include <SFML/Graphics.hpp>

// All sidebar geometry is shared by drawing, mouse handling, and tests.
namespace pathlab_sidebar {
sf::FloatRect viewport(const sf::Vector2u& windowSize);
sf::FloatRect sectionHeader(const sf::Vector2u& size, const PathlabUIData& data,
                            std::size_t section);
sf::FloatRect algorithmSelector(const sf::Vector2u& size, const PathlabUIData& data);
sf::FloatRect algorithmOption(const sf::Vector2u& size, const PathlabUIData& data,
                              std::size_t option);
sf::FloatRect visualizationRow(const sf::Vector2u& size, const PathlabUIData& data,
                               std::size_t row);
sf::FloatRect clearanceSlider(const sf::Vector2u& size, const PathlabUIData& data);
sf::FloatRect clearanceToggle(const sf::Vector2u& size, const PathlabUIData& data);
float maxScroll(const sf::Vector2u& size, const PathlabUIData& data);
bool inSidebar(const sf::Vector2i& pos, const sf::Vector2u& size);
bool inScrollViewport(const sf::Vector2i& pos, const sf::Vector2u& size);
PathlabUIInteraction hitTest(const sf::Vector2i& pos, const sf::Vector2u& size,
                             const PathlabUIData& data);
void draw(sf::RenderWindow& window, const sf::Font& font, const PathlabUIData& data);
}
