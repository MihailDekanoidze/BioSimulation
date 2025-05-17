// File: physics.cpp
#include "../include/Environment.hpp"
#include "../include/Field.hpp"
#include "../include/Parameters.hpp"
#include <SFML/Graphics.hpp>
#include <fstream>

void Environment::addGCell(const GreenCell &cell)
{
    std::shared_ptr<Body> new_body = std::make_unique<GreenCell>(cell);
    new_body->info();
    bodies.push_back(new_body);
}

void Environment::updatePhysics(float dt)
{
    this->removeLowEnergyCells();
    for (auto &body : bodies)
    {
        // Передаем все поля в метод update
        body->update(dt, O2_field, H2_field, C_field, N2_field);
    }
}

void Environment::changeGravity(const sf::Event::KeyEvent &key)
{
    if (key.code == sf::Keyboard::Up)
    {
        setGravity(up_gravity);
    }
    else if (key.code == sf::Keyboard::Down)
    {
        setGravity(down_gravity);
    }
    else if (key.code == sf::Keyboard::Right)
    {
        setGravity(right_gravity);
    }
    else if (key.code == sf::Keyboard::Left)
    {
        setGravity(left_gravity);
    }
    else
    {
    }
    LOG("uvu");
}
void Environment::removeLowEnergyCells()
{
    // Используем erase-remove идиому
    bodies.erase(
        std::remove_if(
            bodies.begin(), bodies.end(),
            [this](const std::shared_ptr<Body> &body)
            {
                // Пытаемся преобразовать Body к Cell
                if (auto cell = std::dynamic_pointer_cast<Cell>(body))
                {
                    if (cell->getEnergy() < 20.0f)
                    {
                        float area_radius = cell->getRaduis() / 2;
                        sf::CircleShape c_cir = sf::CircleShape(area_radius);
                        c_cir.setPosition(cell->getPosition() + sf::Vector2f(cell->getRaduis(), cell->getRaduis()));
                        C_field.applyCircularArea(c_cir, cell->getMass() / (M_PI * area_radius * area_radius), true);

                        sf::CircleShape decay = sf::CircleShape(area_radius * 2);
                        decay.setPosition(cell->getPosition() + sf::Vector2f(cell->getRaduis(), cell->getRaduis()));
                        switch (cell->getDecay())
                        {
                            case O2:
                                O2_field.applyCircularArea(
                                    decay, cell->getEnergy() / 4 / (M_PI * area_radius * area_radius), true);
                                break;
                            case H2:
                                H2_field.applyCircularArea(
                                    decay, cell->getEnergy() / 4 / (M_PI * area_radius * area_radius), true);
                                break;
                            case N2:
                                N2_field.applyCircularArea(
                                    decay, cell->getEnergy() / 4 / (M_PI * area_radius * area_radius), true);
                                break;
                            case C:
                                C_field.applyCircularArea(
                                    decay, cell->getEnergy() / 4 / (M_PI * area_radius * area_radius), true);
                                break;
                            default:
                                LOG("Wrong decay field type ");
                                break;
                        }
                        LOG("Removing cell at (" << cell->getPosition().x << ", " << cell->getPosition().y
                                                 << ") with energy: " << cell->getEnergy());
                        return true;
                    }
                }
                return false;
            }),
        bodies.end());
}