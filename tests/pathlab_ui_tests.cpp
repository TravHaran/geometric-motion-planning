#include <cassert>
#include <cstddef>
#include <iostream>

#include "../src/ui/PathlabUI.hpp"

namespace{

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

    constexpr int optionCenterX = 850;
    constexpr int firstOptionCenterY = 170;
    constexpr int optionHeight = 32;

    for(std::size_t i = 0; i < data.algorithmOptions.size(); ++i){
        const PathlabUIInteraction interaction =
            handlePathlabUIClick(
                {
                    optionCenterX,
                    firstOptionCenterY
                        + optionHeight * static_cast<int>(i)
                },
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

    const PathlabUIInteraction interaction =
        handlePathlabUIClick(
            {850, 138},
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
