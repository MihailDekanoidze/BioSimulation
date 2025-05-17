#pragma once
#include "Parameters.hpp"
#include "Cell.hpp"
#include "Field.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <vector>
#include <memory>
#include "GreenCell.hpp"
#include <algorithm> // для std::clamp


class Environment : virtual protected Log
{
private:
    sf::Vector2f gravity;
    std::vector<std::shared_ptr<Body>> bodies;
    Field H2_field;
    Field O2_field;
    Field N2_field;
    Field C_field;

public:
    /*Environment(sf::Vector2f gr, std::vector<Body>& objects):
        gravity(gr), bodies(objects),
        H2_field(0, H2_color), O2_field(0, O2_color), N2_field(0, N2_color), C_field(0, C_color){}*/
    Environment()
        : gravity(), bodies(), H2_field(width, height, 0, H2_color), O2_field(width, height, 0, O2_color),
          N2_field(width, height, 0, N2_color), C_field(width, height, 0, C_color), Log()
    {
    }
    // Environment(std::vector<Body>& objects):Environment(sf::Vector2f(0, 0), objects){}
    ~Environment() {};
    void addGCell(const GreenCell& cell);
    void drawHField(sf::RenderWindow &window)
    {
        H2_field.render(window);
    }
    void drawOField(sf::RenderWindow &window)
    {
        O2_field.render(window);
    }
    void drawCField(sf::RenderWindow &window)
    {
        C_field.render(window);
    }
    void drawNField(sf::RenderWindow &window)
    {
        N2_field.render(window);
    }
    void drawBodies(sf::RenderWindow &window)
    {
        for (auto it = bodies.begin(); it != bodies.end(); ++it)
        {
            (*it)->draw(window);
        }
    }
    void addCircleH(const sf::CircleShape &area, float value, bool additive = false)
    {
        H2_field.applyCircularArea(area, value, additive);
    }
    void addCircleN(const sf::CircleShape &area, float value, bool additive = false)
    {
        N2_field.applyCircularArea(area, value, additive);
    }
    void addCircleC(const sf::CircleShape &area, float value, bool additive = false)
    {
        C_field.applyCircularArea(area, value, additive);
    }
    void addCircleO(const sf::CircleShape &area, float value, bool additive = false)
    {
        O2_field.applyCircularArea(area, value, additive);
    }
    void updatePhysics(float dt);
    void changeGravity(const sf::Event::KeyEvent &key);
    void setGravity(const sf::Vector2f new_gravity)
    {
        gravity = new_gravity;
    }

    void removeLowEnergyCells();
};
