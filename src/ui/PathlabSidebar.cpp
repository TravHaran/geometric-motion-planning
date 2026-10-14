#include "PathlabSidebar.hpp"
#include "PathlabUILayout.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <string>

namespace pathlab_sidebar {
namespace {
constexpr float HEAD=40.f, GAP=8.f, PAD=18.f;
constexpr sf::Color TEXT(235,240,248), MUTED(151,164,182);
constexpr sf::Color ACCENT(61,135,246);
constexpr sf::Color LINE(58,68,84);
constexpr sf::Color CONTROL(35,43,56);
constexpr std::array<const char*,8> TITLES={{
    "PLANNER","GEOMETRIC FEASIBILITY","VISUALIZATION","SCENE",
    "RESULTS","PERFORMANCE","KINEMATIC REACHABILITY","LIPM DYNAMICS"
}};
bool enabled(const PathlabUIData& d,std::size_t i){
    return i<d.sidebarExpanded.size() && d.sidebarExpanded[i];
}
float bodyHeight(const PathlabUIData& d,std::size_t i){
    switch(i){
        case 0: return 80.f+(d.algorithmDropdownOpen?static_cast<float>(d.algorithmOptions.size())*32.f:0.f);
        case 1: return 132.f;
        case 2: return d.hasSearchTrace?148.f:117.f;
        case 3: return 101.f;
        case 4: return 161.f;
        case 5: return 101.f;
        default:return 37.f;
    }
}
float panelX(const sf::Vector2u& size){
    return static_cast<float>(size.x)-PATHLAB_SIDE_PANEL_WIDTH;
}
float top(){return PATHLAB_TOP_BAR_HEIGHT+110.f;}
float bottom(const sf::Vector2u& s){
    return static_cast<float>(s.y)-PATHLAB_BOTTOM_BAR_HEIGHT-8.f;
}
float yOf(const PathlabUIData& d,std::size_t section){
    float y=top()+6.f;
    for(std::size_t i=0;i<section;++i)
        y+=HEAD+GAP+(enabled(d,i)?bodyHeight(d,i):0.f);
    return y;
}
sf::FloatRect rect(float x,float y,float w,float h){
    return sf::FloatRect(sf::Vector2f(x,y),sf::Vector2f(w,h));
}
sf::FloatRect screenRect(sf::FloatRect r,const PathlabUIData& data){
    r.position.y-=data.sidebarScrollOffset;
    return r;
}
sf::FloatRect contentRect(const sf::Vector2u& s,const PathlabUIData& d,std::size_t i,
                          float offset,float height){
    return screenRect(rect(panelX(s)+PAD,yOf(d,i)+HEAD+offset,
                           PATHLAB_SIDE_PANEL_WIDTH-2*PAD,height),d);
}
void surface(sf::RenderWindow& w, const sf::FloatRect& r,sf::Color fill,
             sf::Color outline=sf::Color::Transparent){
    sf::RectangleShape shape(r.size);
    shape.setPosition(r.position);
    shape.setFillColor(fill);
    shape.setOutlineColor(outline);
    shape.setOutlineThickness(outline==sf::Color::Transparent?0.f:1.f);
    w.draw(shape);
}
void label(sf::RenderWindow& w,const sf::Font& font,const std::string& value,
           float x,float y,unsigned fontSize=12,sf::Color color=TEXT){
    sf::Text t(font);
    t.setString(value);
    t.setCharacterSize(fontSize);
    t.setFillColor(color);
    t.setPosition({x,y});
    w.draw(t);
}
void circle(sf::RenderWindow& w,sf::Vector2f c,float rad,sf::Color fill){
    sf::CircleShape sh(rad);
    sh.setOrigin({rad,rad});sh.setPosition(c);sh.setFillColor(fill);
    w.draw(sh);
}
std::string shortNumber(double value,int precision=2){
    if(!std::isfinite(value))return "-";
    std::string s=std::to_string(value);
    auto p=s.find('.');
    if(p!=std::string::npos && p+1+static_cast<std::size_t>(precision)<s.size())
        s.resize(p+1+static_cast<std::size_t>(precision));
    return s;
}
bool contains(const sf::FloatRect& r,const sf::Vector2i& p){
    return r.contains(sf::Vector2f(static_cast<float>(p.x),static_cast<float>(p.y)));
}
void valueRow(sf::RenderWindow& w,const sf::Font& f,float x,float y,
              const std::string& name,const std::string& value){
    label(w,f,name,x,y,12,MUTED);
    sf::Text t(f);t.setString(value);t.setCharacterSize(12);
    t.setFillColor(TEXT);
    const float width=t.getLocalBounds().size.x;
    t.setPosition({x+240.f-width,y});w.draw(t);
}
void checkRow(sf::RenderWindow& w,const sf::Font& f,float x,float y,
              const char* name,bool selected,bool available){
    surface(w,rect(x,y+5.f,15.f,15.f),
            !available?sf::Color(53,62,77):selected?ACCENT:sf::Color(39,48,61),
            selected?ACCENT:LINE);
    if(selected && available)label(w,f,"✓",x+2.f,y+4.f,12,sf::Color::White);
    label(w,f,name,x+28.f,y+1.f,12,available?TEXT:MUTED);
}
} // namespace

sf::FloatRect viewport(const sf::Vector2u& s){
    return rect(panelX(s),top(),PATHLAB_SIDE_PANEL_WIDTH,
                std::max(1.f,bottom(s)-top()));
}
float maxScroll(const sf::Vector2u& s,const PathlabUIData& d){
    const float content=yOf(d,TITLES.size())-top()+12.f;
    return std::max(0.f,content-viewport(s).size.y);
}
bool inSidebar(const sf::Vector2i& p,const sf::Vector2u& s){
    return p.x>=static_cast<int>(panelX(s)) &&
           p.y>=static_cast<int>(PATHLAB_TOP_BAR_HEIGHT) &&
           p.y<static_cast<int>(bottom(s)+8.f);
}
bool inScrollViewport(const sf::Vector2i& p,const sf::Vector2u& s){
    return contains(viewport(s),p);
}
sf::FloatRect sectionHeader(const sf::Vector2u& s,const PathlabUIData& d,
                            std::size_t i){
    return screenRect(rect(panelX(s)+10.f,yOf(d,i),
                           PATHLAB_SIDE_PANEL_WIDTH-24.f,HEAD),d);
}
sf::FloatRect algorithmSelector(const sf::Vector2u& s,const PathlabUIData& d){
    return contentRect(s,d,0,29.f,31.f);
}
sf::FloatRect algorithmOption(const sf::Vector2u& s,const PathlabUIData& d,
                              std::size_t option){
    return contentRect(s,d,0,60.f+32.f*static_cast<float>(option),32.f);
}
sf::FloatRect visualizationRow(const sf::Vector2u& s,const PathlabUIData& d,
                               std::size_t i){
    return contentRect(s,d,2,8.f+31.f*static_cast<float>(i),29.f);
}
sf::FloatRect clearanceToggle(const sf::Vector2u& s,const PathlabUIData& d){
    return contentRect(s,d,1,6.f,28.f);
}
sf::FloatRect clearanceSlider(const sf::Vector2u& s,const PathlabUIData& d){
    return contentRect(s,d,1,62.f,27.f);
}
PathlabUIInteraction hitTest(const sf::Vector2i& p,const sf::Vector2u& s,
                             const PathlabUIData& d){
    if(!d.sidebarVisible || !inSidebar(p,s))return {};
    if(contains(pathlab_ui_detail::getRunPlannerButtonBounds(s),p))
        return PathlabUIAction::RunPlanner;
    if(!inScrollViewport(p,s))return PathlabUIAction::ConsumeSidebarClick;
    // Dropdown options receive priority over their parent section, but are
    // hit-tested in the same clipped viewport as the rendering.
    if(enabled(d,0) && d.algorithmDropdownOpen){
        for(std::size_t i=0;i<d.algorithmOptions.size();++i){
            if(contains(algorithmOption(s,d,i),p))
                return {PathlabUIAction::SelectAlgorithm,i};
        }
    }
    for(std::size_t i=0;i<TITLES.size();++i){
        if(contains(sectionHeader(s,d,i),p))
            return {PathlabUIAction::ToggleSidebarSection,0,i};
    }
    if(enabled(d,0)&&contains(algorithmSelector(s,d),p))
        return PathlabUIAction::ToggleAlgorithmDropdown;
    if(enabled(d,1)){
        if(contains(clearanceToggle(s,d),p))
            return PathlabUIAction::ToggleClearancePreview;
        if(contains(clearanceSlider(s,d),p))
            return PathlabUIAction::ConsumeSidebarClick;
    }
    if(enabled(d,2)){
        for(std::size_t i=0;i<(d.hasSearchTrace?4u:3u);++i){
            if(contains(visualizationRow(s,d,i),p)){
                switch(i){
                    case 0:return d.hasObstacles?PathlabUIAction::ToggleObstacles:
                                       PathlabUIAction::ConsumeSidebarClick;
                    case 1:return d.hasVisibilityGraph?PathlabUIAction::ToggleVisibilityGraph:
                                       PathlabUIAction::ConsumeSidebarClick;
                    case 2:return d.hasFinalPath?PathlabUIAction::ToggleFinalPath:
                                       PathlabUIAction::ConsumeSidebarClick;
                    default:return PathlabUIAction::ToggleExploredNodes;
                }
            }
        }
    }
    return PathlabUIAction::ConsumeSidebarClick;
}
void draw(sf::RenderWindow& w,const sf::Font& f,const PathlabUIData& d){
    const sf::Vector2u size=w.getSize();
    const float x=panelX(size);
    const auto button=pathlab_ui_detail::getRunPlannerButtonBounds(size);
    // Persistent run control: it doesn't move when settings are scrolled.
    surface(w,rect(x,PATHLAB_TOP_BAR_HEIGHT,PATHLAB_SIDE_PANEL_WIDTH,110.f),
            sf::Color(20,26,36,244));
    surface(w,button,d.canRunPlanner?sf::Color(40,116,228):sf::Color(49,57,70));
    label(w,f,"▶  RUN PLANNER",button.position.x+71.f,
          button.position.y+11.f,14,d.canRunPlanner?sf::Color::White:MUTED);
    label(w,f,"CONFIGURATION  /  SCROLL FOR MORE",x+19.f,
          PATHLAB_TOP_BAR_HEIGHT+87.f,10,MUTED);
    // A clipped SFML view keeps expanded sections inside the panel.
    const sf::View old=w.getView();
    const auto vp=viewport(size);
    sf::View scrollView(sf::FloatRect(
        sf::Vector2f(vp.position.x,vp.position.y+d.sidebarScrollOffset),
        vp.size));
    scrollView.setViewport(sf::FloatRect(
        sf::Vector2f(vp.position.x/static_cast<float>(size.x),
                     vp.position.y/static_cast<float>(size.y)),
        sf::Vector2f(vp.size.x/static_cast<float>(size.x),
                     vp.size.y/static_cast<float>(size.y))));
    w.setView(scrollView);
    // Render in unscrolled coordinates; the SFML view translates and clips.
    PathlabUIData logical=d;
    logical.sidebarScrollOffset=0.f;
    for(std::size_t i=0;i<TITLES.size();++i){
        const auto header=sectionHeader(size,logical,i);
        const bool opened=enabled(d,i);
        surface(w,header,opened?sf::Color(38,49,66):sf::Color(30,39,53));
        label(w,f,TITLES[i],header.position.x+11.f,header.position.y+12.f,
              11,i>=6?MUTED:TEXT);
        label(w,f,opened?"⌄":"›",
              header.position.x+header.size.x-23.f,header.position.y+8.f,
              18,i>=6?MUTED:TEXT);
        if(!opened)continue;
        const float bx=x+PAD, by=header.position.y+HEAD;
        if(i==0){
            label(w,f,"Algorithm",bx,by+5.f,12,MUTED);
            auto selector=algorithmSelector(size,logical);
            surface(w,selector,CONTROL,LINE);
            const auto selected=d.selectedAlgorithmIndex<d.algorithmOptions.size()
                ?d.algorithmOptions[d.selectedAlgorithmIndex]:"Select algorithm";
            label(w,f,selected,selector.position.x+12.f,selector.position.y+7.f,12);
            label(w,f,d.algorithmDropdownOpen?"⌃":"⌄",
                  selector.position.x+selector.size.x-21.f,selector.position.y+5.f,16,MUTED);
            if(d.algorithmDropdownOpen){
                for(std::size_t k=0;k<d.algorithmOptions.size();++k){
                    const auto opt=algorithmOption(size,logical,k);
                    surface(w,opt,k==d.selectedAlgorithmIndex?sf::Color(42,72,112):CONTROL);
                    label(w,f,d.algorithmOptions[k],opt.position.x+11.f,opt.position.y+7.f,12);
                }
            }
        }else if(i==1){
            checkRow(w,f,bx,by+6.f,"Show clearance preview",
                     d.showClearancePreview,true);
            label(w,f,"Obstacle clearance radius",bx,by+39.f,11,MUTED);
            label(w,f,std::to_string(static_cast<int>(d.clearancePreviewRadius))+" units",
                  bx+184.f,by+39.f,11);
            const auto slider=clearanceSlider(size,logical);
            const float mid=slider.position.y+slider.size.y/2.f;
            surface(w,rect(slider.position.x,mid-2.f,slider.size.x,4.f),LINE);
            const float ratio=std::clamp(d.clearancePreviewRadius/120.f,0.f,1.f);
            surface(w,rect(slider.position.x,mid-2.f,slider.size.x*ratio,4.f),
                    sf::Color(247,166,111));
            circle(w,{slider.position.x+slider.size.x*ratio,mid},7.f,
                   sf::Color(255,187,136));
            label(w,f,"Visual only - not enforced by planner",bx,by+101.f,10,MUTED);
        }else if(i==2){
            checkRow(w,f,bx,by+9.f,"Obstacles",d.showObstacles,d.hasObstacles);
            checkRow(w,f,bx,by+40.f,"Visibility Graph",
                     d.showVisibilityGraph,d.hasVisibilityGraph);
            checkRow(w,f,bx,by+71.f,"Final Path",d.showFinalPath,d.hasFinalPath);
            if(d.hasSearchTrace)checkRow(w,f,bx,by+102.f,"Explored Nodes",
                                        d.showExploredNodes,true);
        }else if(i==3){
            valueRow(w,f,bx,by+12.f,"Obstacles",std::to_string(d.obstacleCount));
            valueRow(w,f,bx,by+39.f,"Graph Nodes",std::to_string(d.graphNodeCount));
            valueRow(w,f,bx,by+66.f,"Graph Edges",std::to_string(d.graphEdgeCount));
        }else if(i==4){
            const bool success=d.plannerStatus==PathlabPlannerStatus::PathFound;
            label(w,f,success?"●  PATH FOUND":"●  CHECK PLANNER STATUS",
                  bx,by+7.f,12,success?sf::Color(76,224,157):MUTED);
            valueRow(w,f,bx,by+38.f,"Path Length",shortNumber(d.pathLength)+" px");
            valueRow(w,f,bx,by+65.f,"Path Segments",std::to_string(d.pathSegments));
            valueRow(w,f,bx,by+92.f,"Nodes in Path",std::to_string(d.pathNodes));
            valueRow(w,f,bx,by+119.f,"Nodes Expanded",std::to_string(d.nodesExpanded));
        }else if(i==5){
            valueRow(w,f,bx,by+12.f,"Graph Build",shortNumber(d.graphBuildTimeMs)+" ms");
            valueRow(w,f,bx,by+39.f,"Search",shortNumber(d.searchTimeMs)+" ms");
            valueRow(w,f,bx,by+66.f,"Total",shortNumber(d.totalTimeMs)+" ms");
        }else {
            label(w,f,"Future phase - not implemented",bx,by+9.f,11,MUTED);
        }
    }
    w.setView(old);
    // Subtle scroll rail on the far right.
    const float max=maxScroll(size,d);
    if(max>0.5f){
        const float viewHeight=vp.size.y;
        const float contentHeight=viewHeight+max;
        const float thumb=std::max(28.f,viewHeight*viewHeight/contentHeight);
        const float y=vp.position.y+
            (viewHeight-thumb)*std::clamp(d.sidebarScrollOffset/max,0.f,1.f);
        surface(w,rect(x+PATHLAB_SIDE_PANEL_WIDTH-5.f,vp.position.y,3.f,viewHeight),
                sf::Color(40,51,67));
        surface(w,rect(x+PATHLAB_SIDE_PANEL_WIDTH-5.f,y,3.f,thumb),
                sf::Color(96,114,142));
    }
}
}
