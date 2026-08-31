#include "PathlabApp.hpp"

#include <optional>

PathlabApp::PathlabApp() : window(
    sf::VideoMode({1200, 800}),
    "PATHLAB - Path Planning Visualizer"
){
    window.setMinimumSize(sf::Vector2u{900, 650});
}

void PathlabApp::run(){
    while(window.isOpen()){
        processEvents();
        render();
    }
}

void PathlabApp::processEvents(){
    while(const std::optional event = window.pollEvent()){
        if(event->is<sf::Event::Closed>()){
            window.close();
        }


        if(const auto* resized = event->getIf<sf::Event::Resized>()){
            sf::FloatRect visibleArea({0.0f, 0.0f}, sf::Vector2f(resized->size));
            window.setView(sf::View(visibleArea));
        }
    }
}

void PathlabApp::render(){
    window.clear(sf::Color(18, 20, 24));
    window.display();
}