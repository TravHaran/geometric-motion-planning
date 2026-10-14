#include <SFML/Graphics.hpp>
#include "../src/footstep/PrototypePlanner.hpp"
#include "../src/geometry/Clearance.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {
constexpr float SCALE=62.f, OX=52.f, OY=92.f, PI=3.141592654f;
constexpr float SIDEX=1090.f, SCREENW=1480.f, SCREENH=880.f;
const sf::Color BG(13,18,27), PANEL(23,29,40), TEXT(229,235,243), MUTED(155,168,187);
const sf::Color CYAN(75,198,229), GREEN(83,214,144), BLUE(92,142,244), ORANGE(247,175,81);
sf::Vector2f loc(Point p){return {OX+SCALE*static_cast<float>(p.x),OY+SCALE*static_cast<float>(p.y)};}
float distance(sf::Vector2f a,sf::Vector2f b){return std::hypot(a.x-b.x,a.y-b.y);}
void rect(sf::RenderTarget& t,float x,float y,float w,float h,sf::Color color){
    sf::RectangleShape r({w,h});r.setPosition({x,y});r.setFillColor(color);t.draw(r);
}
void circle(sf::RenderTarget& t,sf::Vector2f p,float radius,sf::Color color){
    sf::CircleShape c(radius);c.setOrigin({radius,radius});c.setPosition(p);c.setFillColor(color);t.draw(c);
}
void line(sf::RenderTarget& t,sf::Vector2f a,sf::Vector2f b,sf::Color color,float width=2.f){
    sf::Vector2f v=b-a;
    if(distance(a,b)<.001f)return;
    sf::RectangleShape segment({distance(a,b),width});
    segment.setOrigin({0.f,width/2.f});
    segment.setPosition(a);
    segment.setRotation(sf::radians(std::atan2(v.y,v.x)));
    segment.setFillColor(color);t.draw(segment);
}
void text(sf::RenderTarget& t,const sf::Font& font,const std::string& s,float x,float y,
          unsigned size=15,sf::Color color=TEXT){
    sf::Text label(font);label.setString(s);label.setCharacterSize(size);
    label.setFillColor(color);label.setPosition({x,y});t.draw(label);
}
std::string format(double v,int precision=2){
    std::ostringstream s;s<<std::fixed<<std::setprecision(precision)<<v;return s.str();
}
void fillPolygon(sf::RenderTarget& t,const Polygon& poly,sf::Color color){
    sf::ConvexShape shape(poly.vertices.size());
    for(std::size_t i=0;i<poly.vertices.size();++i)shape.setPoint(i,loc(poly.vertices[i]));
    shape.setFillColor(color);t.draw(shape);
}
void strokePolygon(sf::RenderTarget& t,const Polygon& poly,sf::Color color){
    for(std::size_t i=0;i<poly.vertices.size();++i)
        line(t,loc(poly.vertices[i]),loc(poly.vertices[(i+1)%poly.vertices.size()]),color,1.5f);
}
void footprint(sf::RenderTarget& t,Point p,double yaw,sf::Color color){
    sf::RectangleShape foot({26.f,12.f});
    foot.setOrigin({13.f,6.f});foot.setPosition(loc(p));
    foot.setRotation(sf::radians(static_cast<float>(yaw)));
    foot.setFillColor(color);t.draw(foot);
}
class Demo {
public:
    Demo():window(sf::VideoMode({static_cast<unsigned>(SCREENW),static_cast<unsigned>(SCREENH)}),
                  "PATHLAB | Hierarchical Biped Planner Prototype"){
        window.setFramerateLimit(60);
        loaded=font.openFromFile("assets/fonts/InterVariable.ttf");
        if(!loaded)std::cerr<<"Missing assets/fonts/InterVariable.ttf\n";
        replan();
    }
    void run(){
        sf::Clock clock;
        while(window.isOpen()){
            float dt=clock.restart().asSeconds();
            while(const auto event=window.pollEvent())handle(*event);
            if(playing){
                float length=static_cast<float>(segments());
                progress=std::min(length,progress+dt*2.2f);
                if(progress>=length)playing=false;
            }
            paint();
        }
    }
private:
    sf::RenderWindow window;sf::Font font;bool loaded=false;
    footstep::Settings config;
    footstep::Scene scene=footstep::sampleScene();
    footstep::Layer mode=footstep::Layer::Geometry;
    footstep::Result result;
    bool playing=false,obstructed=true,dragTimeline=false;
    int dragControl=-1;
    float progress=0;
    int segments()const {
        if(mode==footstep::Layer::Geometry)
            return std::max(0,static_cast<int>(result.geometricPath.size())-1);
        return std::max(0,static_cast<int>(result.footsteps.size())-1);
    }
    void replan(){
        playing=false;progress=0;
        scene=footstep::sampleScene(obstructed);
        result=footstep::solve(scene,config,mode);
    }
    void setSlider(int index,float x){
        double t=std::clamp((x-1130.f)/280.f,0.f,1.f);
        if(index==0)config.clearance=.06+.64*t;
        if(index==1)config.maxStep=.45+.60*t;
        if(index==2)config.stepTime=.25+.25*t;
        replan();
    }
    void scrub(float x){
        float t=std::clamp((x-80.f)/960.f,0.f,1.f);
        progress=t*static_cast<float>(segments());playing=false;
    }
    void handle(const sf::Event& event){
        if(event.is<sf::Event::Closed>()){window.close();return;}
        if(const auto* k=event.getIf<sf::Event::KeyPressed>()){
            using Key=sf::Keyboard::Key;
            if(k->code==Key::Escape)window.close();
            if(k->code==Key::Space)playing=!playing;
            if(k->code==Key::Num1){mode=footstep::Layer::Geometry;replan();}
            if(k->code==Key::Num2){mode=footstep::Layer::Kinematic;replan();}
            if(k->code==Key::Num3){mode=footstep::Layer::Dynamic;replan();}
            if(k->code==Key::R)replan();
            if(k->code==Key::M){obstructed=!obstructed;replan();}
            if(k->code==Key::Right){playing=false;progress=std::min(static_cast<float>(segments()),progress+1.f);}
            if(k->code==Key::Left){playing=false;progress=std::max(0.f,progress-1.f);}
        }
        if(const auto* e=event.getIf<sf::Event::MouseButtonPressed>()){
            if(e->button!=sf::Mouse::Button::Left)return;
            float x=static_cast<float>(e->position.x),y=static_cast<float>(e->position.y);
            if(x>80.f&&x<1040.f&&y>802.f&&y<834.f){dragTimeline=true;scrub(x);return;}
            if(x>1115.f&&x<1435.f){
                if(y>121&&y<164){mode=footstep::Layer::Geometry;replan();}
                else if(y>170&&y<214){mode=footstep::Layer::Kinematic;replan();}
                else if(y>219&&y<263){mode=footstep::Layer::Dynamic;replan();}
                else if(y>354&&y<387){dragControl=0;setSlider(0,x);}
                else if(y>454&&y<487){dragControl=1;setSlider(1,x);}
                else if(y>554&&y<587){dragControl=2;setSlider(2,x);}
                else if(y>660&&y<706)playing=!playing;
                else if(y>718&&y<767){obstructed=!obstructed;replan();}
            }
        }
        if(const auto* e=event.getIf<sf::Event::MouseMoved>()){
            if(dragTimeline)scrub(static_cast<float>(e->position.x));
            if(dragControl>=0)setSlider(dragControl,static_cast<float>(e->position.x));
        }
        if(event.is<sf::Event::MouseButtonReleased>()){
            dragTimeline=false;dragControl=-1;
        }
    }
    void button(float y,const std::string& name,bool selected){
        rect(window,1116,y,329,43,selected?sf::Color(45,78,117):sf::Color(35,44,58));
        text(window,font,name,1131,y+9,16,selected?sf::Color::White:MUTED);
    }
    void slider(float y,const std::string& title,double value,double a,double b,const std::string& displayed){
        text(window,font,title,1124,y-43,15,MUTED);
        text(window,font,displayed,1348,y-43,15,TEXT);
        rect(window,1130,y,280,4,sf::Color(76,89,106));
        float x=1130+280*static_cast<float>((value-a)/(b-a));
        rect(window,1130,y,std::max(0.f,x-1130),4,CYAN);
        circle(window,{x,y+2.f},8,CYAN);
    }
    void paint(){
        window.clear(BG);
        rect(window,0,0,SIDEX,SCREENH,sf::Color(17,24,34));
        rect(window,SIDEX,0,SCREENW-SIDEX,SCREENH,PANEL);
        if(loaded){
            text(window,font,"PATHLAB",50,20,24);
            text(window,font,"GEOMETRIC  >  KINEMATIC  >  LIPM",205,27,13,MUTED);
        }
        for(int i=0;i<=16;++i)line(window,loc({static_cast<double>(i),0}),loc({static_cast<double>(i),10}),sf::Color(43,52,66),1);
        for(int i=0;i<=10;++i)line(window,loc({0,static_cast<double>(i)}),loc({16,static_cast<double>(i)}),sf::Color(43,52,66),1);
        // The obstacle exclusion band is the Minkowski sum with a disk:
        // polygon interior + segment capsules along all polygon edges.
        for(const auto& p:scene.obstacles){
            float radius=static_cast<float>(config.clearance*SCALE);
            for(std::size_t i=0;i<p.vertices.size();++i){
                auto a=loc(p.vertices[i]),b=loc(p.vertices[(i+1)%p.vertices.size()]);
                line(window,a,b,sf::Color(236,101,100,65),2*radius);
                circle(window,a,radius,sf::Color(236,101,100,65));
            }
            fillPolygon(window,p,sf::Color(110,62,66));
            strokePolygon(window,p,sf::Color(248,134,131));
        }
        // Route from Stage 1 is retained as a faint geometric reference.
        if(result.geometricPath.size()>1){
            for(std::size_t i=1;i<result.geometricPath.size();++i){
                line(window,loc(result.geometricPath[i-1]),loc(result.geometricPath[i]),
                     mode==footstep::Layer::Geometry?CYAN:sf::Color(71,130,147,85),2.6f);
            }
        }
        circle(window,loc(scene.start),7,GREEN);circle(window,loc(scene.goal),8,ORANGE);
        if(loaded){
            text(window,font,"S",loc(scene.start).x-4,loc(scene.start).y-30,16,GREEN);
            text(window,font,"G",loc(scene.goal).x-5,loc(scene.goal).y-31,16,ORANGE);
        }
        if(mode==footstep::Layer::Geometry && result.geometricPath.size()>1){
            int n=segments(), index=std::clamp(static_cast<int>(progress),0,n-1);
            for(int i=0;i<index;++i)line(window,loc(result.geometricPath[static_cast<std::size_t>(i)]),
                                       loc(result.geometricPath[static_cast<std::size_t>(i+1)]),GREEN,4.f);
            float frac=std::clamp(progress-static_cast<float>(index),0.f,1.f);
            Point a=result.geometricPath[static_cast<std::size_t>(index)],b=result.geometricPath[static_cast<std::size_t>(index+1)];
            Point p{a.x+(b.x-a.x)*frac,a.y+(b.y-a.y)*frac};
            circle(window,loc(p),9,CYAN);
        }
        if(mode!=footstep::Layer::Geometry && !result.footsteps.empty()){
            int count=static_cast<int>(result.footsteps.size());
            int finished=std::clamp(static_cast<int>(progress),0,count-1);
            Point previous=result.footsteps.front().support;
            for(int i=0;i<count;++i){
                const auto& s=result.footsteps[static_cast<std::size_t>(i)];
                if(i>0)line(window,loc(previous),loc(s.support),
                             i<=finished?GREEN:sf::Color(124,147,177,100),2);
                footprint(window,s.support,s.heading,
                          i<=finished?((i%2)?ORANGE:BLUE):sf::Color(96,109,126,85));
                previous=s.support;
            }
            const auto& current=result.footsteps[static_cast<std::size_t>(finished)];
            // Upper bound disk for reachability; actual feasible region has further restrictions.
            sf::CircleShape disk(static_cast<float>(config.maxStep*SCALE),64);
            float rad=static_cast<float>(config.maxStep*SCALE);
            disk.setOrigin({rad,rad});disk.setPosition(loc(current.support));
            disk.setFillColor(sf::Color(71,192,212,20));disk.setOutlineThickness(1);
            disk.setOutlineColor(sf::Color(88,206,219,110));window.draw(disk);
            if(finished<count-1){
                double frac=std::clamp(static_cast<double>(progress-finished),0.0,1.0);
                Point c=current.support;
                if(mode==footstep::Layer::Dynamic)
                    c=footstep::propagateLIPM(current,config.stepTime*frac,config.comHeight).com;
                else{
                    Point b=result.footsteps[static_cast<std::size_t>(finished+1)].support;
                    c={c.x+(b.x-c.x)*frac,c.y+(b.y-c.y)*frac};
                }
                circle(window,loc(c),8,GREEN);
                if(mode==footstep::Layer::Dynamic){
                    Point xi=footstep::capturePoint(
                        footstep::propagateLIPM(current,config.stepTime*frac,config.comHeight),config.comHeight);
                    circle(window,loc(xi),5,ORANGE);
                }
            }else circle(window,loc(current.com),8,GREEN);
        }
        rect(window,49,783,1000,65,sf::Color(25,35,48));
        text(window,font,"PLAN PLAYBACK",80,786,12,MUTED);
        rect(window,80,821,960,5,sf::Color(87,103,125));
        float playX=80+960*(segments()>0?progress/static_cast<float>(segments()):0.f);
        rect(window,80,821,playX-80,5,GREEN);
        circle(window,{playX,823},8,GREEN);
        if(loaded){
            text(window,font,"BIPED PROTOTYPE",1120,27,22);
            text(window,font,"1   GEOMETRIC CLEARANCE",1124,93,14,MUTED);
            button(121,"1  Geometric path",mode==footstep::Layer::Geometry);
            button(170,"2  Kinematic footsteps",mode==footstep::Layer::Kinematic);
            button(219,"3  LIPM feasibility",mode==footstep::Layer::Dynamic);
            slider(370,"Clearance radius",config.clearance,.06,.70,format(config.clearance)+" m");
            slider(470,"Max footstep length",config.maxStep,.45,1.05,format(config.maxStep)+" m");
            slider(570,"Step duration",config.stepTime,.25,.50,format(config.stepTime)+" s");
            button(660,playing?"Pause playback":"Play / resume",playing);
            button(718,obstructed?"Map: obstacles ON":"Map: open space",false);
            text(window,font,result.reachedGoal?"GOAL REACHED":"NO COMPLETE PLAN",1120,794,16,
                 result.reachedGoal?GREEN:ORANGE);
            text(window,font,"Expanded: "+std::to_string(result.expanded),1120,824,13,MUTED);
            text(window,font,"Steps/segments: "+std::to_string(segments()),1280,824,13,MUTED);
            text(window,font,"Space: play    Arrows: step    M: map    R: replan",50,854,12,MUTED);
            if(mode==footstep::Layer::Dynamic){
                text(window,font,"LIPM uses fixed CoP and step timing.",1120,613,12,MUTED);
                text(window,font,"DCM screening is not full capturability.",1120,631,12,MUTED);
            }
        }
        window.display();
    }
};
}
int main(){Demo app;app.run();}
