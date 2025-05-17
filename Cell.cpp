#include "../include/Cell.hpp"

sf::Vector2f &Body::getPosition()
{
    return position;
}
sf::Vector2f &Body::getVelocity()
{
    return velocity;
}
float Body::getMass() const
{
    return mass;
}

void Cell::draw(sf::RenderWindow &window)
{
    LOG("Velocity " << this->velocity.x << " " << this->velocity.y);
    window.draw(shape);
}

void Cell::initializeCheckPoints(float sensorRadius)
{
    checkPoints.clear();
    const int cells = static_cast<int>(sensorRadius);

    for (int dx = -cells; dx <= cells; dx += step)
    {
        for (int dy = -cells; dy <= cells; dy += step)
        {
            checkPoints.emplace_back(dx, dy);
        }
    }
}

void Cell::updateColor()
{
    float energy_ratio = energy / 100.0f;
    shape.setFillColor(sf::Color(static_cast<sf::Uint8>(basic_color.r * energy_ratio),
                                 static_cast<sf::Uint8>(basic_color.g * energy_ratio),
                                 static_cast<sf::Uint8>(basic_color.b * energy_ratio)));
}

void Cell::info(void)
{
    LOG("Body on position " << (shape.getPosition().x, shape.getPosition().y) << " size: " << shape.getRadius()
                            << " with Color " << static_cast<unsigned>(shape.getFillColor().r)
                            << static_cast<unsigned>(shape.getFillColor().g)
                            << static_cast<unsigned>(shape.getFillColor().b));
    LOG("Cell properties - Speed: " << speed << " Energy: " << energy << " Shell: " << shell);
}

/*void Cell::updatePosterity(void){
    if (mass >= div_mass){
        sf::Vector2f AbsPos = position + sf::Vector2f(shape.getRadius(), shape.getRadius());

        if (AbsPos.x + 3 * shape.getPosition().x > x_offset && AbsPos.x + 3 * shape.getPosition().x < width - x_offset)
    }
}*/

// ... остальной код

void Cell::updateBasic(float dt)
{
    // Применяем ускорение
    if (mass > 0)
    {
        acceleration = force / mass;
    }
    else
    {
        acceleration = {0, 0}; // Защита от деления на ноль
    }
    velocity += acceleration * dt;

    // Ограничение скорости
    float current_speed = std::hypot(velocity.x, velocity.y);
    if (current_speed > 0 && current_speed > speed)
    {
        velocity = (velocity / current_speed) * speed;
    }

    // Обновление позиции
    position += velocity * dt;
    shape.setPosition(position);

    // Обновление цвета
    updateColor();

    // Обработка границ
    sf::Vector2f pos = position;
    if (pos.x < 7 || pos.x > width - 17 - shape.getRadius())
    {
        velocity.x *= -1;
        acceleration.x *= -1;
    }
    if (pos.y < 7 || pos.y > height - y_offset - shape.getRadius())
    {
        velocity.y *= -1;
        acceleration.y *= -1;
    }
}

void Cell::setTargetVelocity(const sf::Vector2f &target_vel)
{
    if (mass > 0)
    {
        sf::Vector2f desired_force = mass * (target_vel - velocity);
        applyForce(desired_force);
    }
}