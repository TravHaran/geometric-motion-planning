#pragma once

#include <SFML/Graphics.hpp>

/**
 * This represents the actual application
 * it has three responsibilities
 * run()
 *      application loop
 * processEvents()
 *      keyboard/window/mouse events
 * render()
 *      draw a frame
 */

class PathlabApp{
    public:
        PathlabApp();
        void run();
    
    private:
        void processEvents();

        void render();

        sf::RenderWindow window;
};