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

    for (size_t i = 0; i < bodies.size(); ++i)
    {
        auto cell1 = std::dynamic_pointer_cast<Cell>(bodies[i]);
        if (cell1)
        {
            cell1->setForce({0, 0});

            cell1->update(dt, O2_field, H2_field, C_field, N2_field, bodies);
            this->applyViscousFriction(*cell1);
            LOG("Cell1 force " << cell1->getForce().x << " " << cell1->getForce().y);
        }
        for (size_t j = i + 1; j < bodies.size(); ++j)
        {
            // Проверка на nullptr
            if (!bodies[i] || !bodies[j])
                continue;

            // Приведение Body к Cell
            auto cell2 = std::dynamic_pointer_cast<Cell>(bodies[j]);

            // Если оба объекта - клетки

            if (cell2)
                handleElasticCollision(*cell1, *cell2);
        }
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

bool Environment::checkCollision(Cell &a, Cell &b) const
{
    sf::Vector2f pos_a = a.getPosition() - sf::Vector2f(a.getRaduis(), a.getRaduis());
    sf::Vector2f pos_b = b.getPosition() - sf::Vector2f(b.getRaduis(), b.getRaduis());
    sf::Vector2f delta = pos_a - pos_b;
    float distance = std::hypot(delta.x, delta.y);
    return (distance < a.getRaduis() + b.getRaduis());
}

void Environment::handleElasticCollision(Cell &a, Cell &b)
{
    if (!checkCollision(a, b))
        return;

    sf::Vector2f pos_a = a.getPosition() - sf::Vector2f(a.getRaduis(), a.getRaduis());
    sf::Vector2f pos_b = b.getPosition() - sf::Vector2f(b.getRaduis(), b.getRaduis());

    // Вектор столкновения (от a к b)
    sf::Vector2f collision_vector = pos_b - pos_a;
    float distance = std::hypot(collision_vector.x, collision_vector.y);
    if (distance == 0)
        return; // Защита от деления на ноль

    // Нормализация вектора столкновения
    sf::Vector2f collision_normal = collision_vector / distance;

    // Относительная скорость
    sf::Vector2f relative_velocity = a.getVelocity() - b.getVelocity();

    // Скалярное произведение относительной скорости и нормали
    float velocity_along_normal = relative_velocity.x * collision_normal.x + relative_velocity.y * collision_normal.y;

    // Если объекты удаляются друг от друга, столкновение не обрабатывается
    if (velocity_along_normal > 0)
        return;

    // Коэффициент упругости (1.0 для абсолютно упругого удара)
    float elasticity = 1.0f;

    // Импульс
    float impulse = -(1.0f + elasticity) * velocity_along_normal;
    impulse /= (1.0f / a.getMass() + 1.0f / b.getMass());

    // Применение импульса к скоростям
    sf::Vector2f impulse_vector = impulse * collision_normal;
    a.setVelocity(a.getVelocity() + impulse_vector / a.getMass());
    b.setVelocity(b.getVelocity() - impulse_vector / b.getMass());
    // Корректировка позиций, чтобы избежать "залипания"
    const float overlap = (a.getRaduis() + b.getRaduis() - distance) / 2.0f;
    a.setPosition(a.getPosition() - overlap * collision_normal);
    b.setPosition(b.getPosition() + overlap * collision_normal);
}

void Environment::applyViscousFriction(Cell &a)
{
    sf::Vector2f friction_force = -friction_coefficient * a.getVelocity();
    a.addForce(friction_force);
    LOG("friction_force " << friction_force.x << " " << friction_force.y);
}

bool Environment::isPositionFree(const sf::Vector2f &pos, float radius) const
{
    for (const auto &body : bodies)
    {
        if (auto cell = dynamic_cast<Cell *>(body.get()))
        {
            float distance = std::hypot(cell->getPosition().x - pos.x, cell->getPosition().y - pos.y);
            if (distance < (radius + cell->getRaduis()))
            {
                return false;
            }
        }
    }
    return true;
}