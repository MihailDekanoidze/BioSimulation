#include "../include/Cell.hpp"
#include "../include/Parameters.hpp"
#include <cmath>

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
    // LOG("Velocity " << this->velocity.x << " " << this->velocity.y);
    window.draw(shape);
}

void Cell::setDir(const sf::Vector2f dir)
{
    LOG("Changing dir from " << direction.x << " " << direction.y << " to " << dir.x << " " << dir.y);
    if (Abs(dir.x) < epsilon && Abs(dir.y) < epsilon)
    {
        direction = sf::Vector2f(1, 0);
    }
    else
    {
        direction = dir / static_cast<float>(hypot(dir.x, dir.y));
    }
    LOG("Now direction " << direction.x << " " << direction.y);
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
    LOG("Cell properties - force: " << max_force << " Energy: " << energy << " Shell: " << shell);
}

/*void Cell::updatePosterity(void){
    if (mass >= div_mass){
        sf::Vector2f AbsPos = position + sf::Vector2f(shape.getRadius(), shape.getRadius());

        if (AbsPos.x + 3 * shape.getPosition().x > x_offset && AbsPos.x + 3 * shape.getPosition().x < width - x_offset)
    }
}*/

// ... остальной код

bool Cell::updateBasic(float dt, std::vector<std::shared_ptr<Body>> &bodies)
{
    // Применяем ускорение
    this->setOwnForce();

    if (mass > 0)
    {
        acceleration = force / mass;
        // LOG("Acceleration " << acceleration.x << " " << acceleration.y);
    }
    else
    {
        acceleration = {0, 0}; // Защита от деления на ноль
    }
    velocity += acceleration * dt;
    // LOG("Velocity " << velocity.x << " " << velocity.y);

    position += velocity * dt;
    shape.setPosition(position);

    // Обновление цвета
    updateColor();

    // Обработка границ
    sf::Vector2f pos = position;
    bool is_overwent = false;
    if (pos.x < 7 || pos.x > width - 17 - shape.getRadius())
    {
        velocity.x *= -1;
        acceleration.x *= -1;
        is_overwent = true;
    }
    if (pos.y < 7 || pos.y > height - y_offset - shape.getRadius())
    {
        velocity.y *= -1;
        acceleration.y *= -1;
        is_overwent = true;
    }
    if (is_overwent)
        this->setDir(-getDir());

    static float divide_timer = 0.0f;
    divide_timer += dt;
    if (is_divided)
    {
        if (divide_timer >= divide_cooldown)
        {
            if (tryDivide(bodies))
            {
                divide_timer = 0.0f;
                is_divided = true;
            }
        }
    }
    else
    {
        if (divide_timer >= initial_div_time)
        {
            if (tryDivide(bodies))
            {
                divide_timer = 0.0f;
            }
        }
    }

    return is_overwent;
}

void Cell::setOwnForce(void)
{
    if (mass > 0)
    {
        sf::Vector2f normal_vel = velocity / static_cast<float>(hypot(velocity.x, velocity.y));
        sf::Vector2f normal_dir = direction / static_cast<float>(hypot(direction.x, direction.y));

        // LOG("normal_vel " << normal_vel.x << " " << normal_vel.y);
        // LOG("normal_dir " << normal_dir.x << " " << normal_dir.y);

        if (Abs(normal_dir.x - normal_vel.x) < veldir_accuraty && Abs(normal_dir.y - normal_vel.x) < veldir_accuraty)
        {
            this->addForce(max_force * normal_dir);
            // LOG("\n Go streight");
        }
        else
        {
            sf::Vector2f force_vec = normal_dir - normal_vel;
            // LOG("Force direction " << force_vec.x << " " << force_vec.y);
            force_vec = force_vec / static_cast<float>(hypot(force_vec.x, force_vec.y));
            // LOG("Normal force direction " << force_vec.x << " " << force_vec.y);

            this->addForce(max_force * force_vec);
            // LOG("Added " << max_force * force_vec.x << " " << max_force * force_vec.y);
            // LOG("\n Chaning dir");
        }
    }
}

bool Cell::isPositionFree(const sf::Vector2f &pos, float radius, const std::vector<std::shared_ptr<Body>> &bodies)
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