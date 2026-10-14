#include <cassert>
#include <cstddef>
#include <iostream>

#include "../src/ui/PathlabUI.hpp"
#include "../src/ui/PathlabSidebar.hpp"
#include "../src/ui/PathlabUILayout.hpp"

namespace {
sf::Vector2i centerOf(const sf::FloatRect& r){
    return {static_cast<int>(r.position.x+r.size.x/2.0f),
            static_cast<int>(r.position.y+r.size.y/2.0f)};
}
PathlabUIData sample(){
    PathlabUIData data;
    data.sidebarVisible=true;
    data.algorithmOptions={"BFS","Dijkstra","A*","Future"};
    data.hasObstacles=true;
    data.hasFinalPath=true;
    data.hasVisibilityGraph=true;
    return data;
}
void testStickyRunButton(){
    const sf::Vector2u size{1200,800};
    auto d=sample();
    const auto fixed=pathlab_ui_detail::getRunPlannerButtonBounds(size);
    assert(fixed.position.y<pathlab_sidebar::viewport(size).position.y);
    assert(handlePathlabUIClick(centerOf(fixed),size,d).action ==
           PathlabUIAction::RunPlanner);
    d.sidebarScrollOffset=pathlab_sidebar::maxScroll(size,d);
    assert(handlePathlabUIClick(centerOf(fixed),size,d).action ==
           PathlabUIAction::RunPlanner);
}
void testExpandableSections(){
    const sf::Vector2u size{1200,900};
    auto d=sample();
    auto header=pathlab_sidebar::sectionHeader(size,d,1);
    auto hit=handlePathlabUIClick(centerOf(header),size,d);
    assert(hit.action==PathlabUIAction::ToggleSidebarSection);
    assert(hit.sectionIndex==1);
    d.sidebarExpanded[1]=false;
    // Old slider bounds must not respond when geometry is collapsed.
    const auto newHeader=pathlab_sidebar::sectionHeader(size,d,2);
    assert(newHeader.position.y<header.position.y+100.f);
}
void testAlgorithmDropdownScroll(){
    const sf::Vector2u size{1200,900};
    auto d=sample();
    auto hit=handlePathlabUIClick(
        centerOf(pathlab_sidebar::algorithmSelector(size,d)),size,d);
    assert(hit.action==PathlabUIAction::ToggleAlgorithmDropdown);
    d.algorithmDropdownOpen=true;
    for(std::size_t i=0;i<d.algorithmOptions.size();++i){
        auto bounds=pathlab_sidebar::algorithmOption(size,d,i);
        auto click=handlePathlabUIClick(centerOf(bounds),size,d);
        assert(click.action==PathlabUIAction::SelectAlgorithm);
        assert(click.algorithmIndex==i);
    }
}
void testScrollAndClipping(){
    const sf::Vector2u size{1200,650};
    auto d=sample();
    const float max=pathlab_sidebar::maxScroll(size,d);
    assert(max>0.f);
    d.sidebarScrollOffset=max;
    const auto v=pathlab_sidebar::viewport(size);
    assert(pathlab_sidebar::inScrollViewport(centerOf(v),size));
    // A scrolled-out header must not receive an accidental click.
    auto old=pathlab_sidebar::sectionHeader(size,d,0);
    assert(old.position.y<v.position.y);
    auto click=handlePathlabUIClick(centerOf(v),size,d);
    assert(click.action!=PathlabUIAction::None);
    // Sidebar hits should never become obstacle-placement canvas clicks.
    auto background=sf::Vector2i{static_cast<int>(size.x)-12,
                                 static_cast<int>(v.position.y+12.f)};
    assert(handlePathlabUIClick(background,size,d).action!=PathlabUIAction::None);
}
void testClearanceUI(){
    const sf::Vector2u size{1200,900};
    auto d=sample();
    assert(handlePathlabUIClick(
        centerOf(pathlab_sidebar::clearanceToggle(size,d)),size,d).action ==
        PathlabUIAction::ToggleClearancePreview);
    assert(handlePathlabUIClick(
        centerOf(pathlab_sidebar::clearanceSlider(size,d)),size,d).action ==
        PathlabUIAction::ConsumeSidebarClick);
    d.sidebarExpanded[1]=false;
    auto point=centerOf(pathlab_sidebar::clearanceSlider(size,d));
    assert(handlePathlabUIClick(point,size,d).action !=
           PathlabUIAction::ToggleClearancePreview);
}
}
int main(){
    testStickyRunButton();
    testExpandableSections();
    testAlgorithmDropdownScroll();
    testScrollAndClipping();
    testClearanceUI();
    std::cout<<"Scrollable PATHLAB sidebar UI tests passed.\n";
}
