#pragma once
#include "Environment.hpp"
#include "Parameters.hpp"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>


class Simulation : protected Log
{
private:
    bool is_pause;
    bool show_O2 = false;
    bool show_H2 = false;
    bool show_C = false;
    bool show_N2 = false;
    sf::Font font;
protected:
    sf::RenderWindow window;
    Environment env;
    sf::Clock clock;

public:
    Simulation() : window(sf::VideoMode(1920, 1080, 64U), "Physics Simulation"), clock(), 
    is_pause(false), show_O2(false), show_H2(false), show_C(false), show_N2(false), font(),  Log()
    {
        window.setFramerateLimit(60);
        if (!font.loadFromFile("DejaVuSans.ttf"))
        {
            std::cerr << "Fail" << std::endl;
        }
        Environment env();
    };
    ~Simulation() {};
    void init();
    void run();
    void pause(const sf::Event::KeyEvent &key);
    void handlePauseMenuInput(const sf::Event::KeyEvent& key);
    void drawPauseMenu();
};
