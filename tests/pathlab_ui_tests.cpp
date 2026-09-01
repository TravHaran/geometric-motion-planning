#include <cassert>
#include <cstddef>
#include <iostream>

#include "../src/ui/PathlabUI.hpp"
#include "../src/ui/PathlabUILayout.hpp"

namespace{

sf::Vector2i centerOf(const sf::FloatRect& bounds){
    return {
        static_cast<int>(bounds.position.x + bounds.size.x / 2.0f),
        static_cast<int>(bounds.position.y + bounds.size.y / 2.0f)
    };
}

void testAlgorithmOptionsAreSelectedByIndex(){
    const sf::Vector2u windowSize{1000, 800};

    PathlabUIData data;
    data.sidebarVisible = true;
    data.algorithmDropdownOpen = true;
    data.algorithmOptions = {
        "First",
        "Second",
        "Third",
        "Future planner"
    };

    for(std::size_t i = 0; i < data.algorithmOptions.size(); ++i){
        const sf::FloatRect optionBounds =
            pathlab_ui_detail::getAlgorithmOptionBounds(
                windowSize,
                i
            );

        const PathlabUIInteraction interaction =
            handlePathlabUIClick(
                centerOf(optionBounds),
                windowSize,
                data
            );

        assert(interaction.action == PathlabUIAction::SelectAlgorithm);
        assert(interaction.algorithmIndex == i);
    }
}

void testAlgorithmSelectorToggleRemainsSeparate(){
    const sf::Vector2u windowSize{1000, 800};

    PathlabUIData data;
    data.sidebarVisible = true;
    data.algorithmDropdownOpen = false;
    data.algorithmOptions = {"BFS", "Dijkstra", "A*"};

    const sf::FloatRect selectorBounds =
        pathlab_ui_detail::getAlgorithmSelectorBounds(windowSize);

    const PathlabUIInteraction interaction =
        handlePathlabUIClick(
            centerOf(selectorBounds),
            windowSize,
            data
        );

    assert(interaction.action == PathlabUIAction::ToggleAlgorithmDropdown);
}

}

int main(){
    testAlgorithmOptionsAreSelectedByIndex();
    testAlgorithmSelectorToggleRemainsSeparate();

    std::cout << "All PATHLAB UI tests passed.\n";

    return 0;
}
