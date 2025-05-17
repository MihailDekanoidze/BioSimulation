#include "../include/GreenCell.hpp"

bool GreenCell::update(float dt, Field &O2_field, Field &H2_field, Field &C_field, Field &N2_field)
{
    // Расчет градиента O2
    sf::Vector2f grad = calculateO2Gradient(O2_field);

    // Если градиент нулевой, активируем случайное движение
    if (grad.x == 0 && grad.y == 0)
    {
        if (random_move_timer <= 0)
        {
            generateNewRandomDirection();
            random_move_timer = random_move_duration;
        }
        setTargetVelocity(random_direction * speed);
        random_move_timer -= dt;
    }
    else
    {
        setTargetVelocity(grad * speed);
        random_move_timer = 0.0f;
    }

    // Обновление энергии и позиции
    energy -= enegry_consume * dt;
    float total_consumed = consumeO2(O2_field, dt);
    energy += total_consumed * energy_conversion;
    if (energy > capacity)
    {
        mass += (energy - capacity) * enegry_to_mass;
        energy = capacity;
    }

    // Ограничение энергии
    energy = std::clamp(energy, 0.0f, 100.0f);

    shell = std::max(0.0f, shell - 0.05f * dt);
    updateColor();
    this->updateBasic(dt);
    return true;
}

void GreenCell::generateNewRandomDirection()
{
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    random_direction = sf::Vector2f(dist(rng), dist(rng));
    float length = std::hypot(random_direction.x, random_direction.y);
    if (length > 0)
        random_direction /= length;
}

sf::Vector2f GreenCell::calculateO2Gradient(const Field &O2_field)
{
    sf::Vector2f gradient(0, 0);
    for (const auto &offset : checkPoints)
    {
        sf::Vector2f samplePos = getPosition() + offset + sf::Vector2f(shape.getRadius(), shape.getRadius());
        if (O2_field.isInside(samplePos))
        {
            gradient += offset * O2_field.getValue(samplePos) /
                        (sqrtf(((offset.x + 0.1)) * (offset.x + 0.1) * (offset.x + 0.1) * (offset.x + 0.1) +
                               (offset.y + 0.1) * (offset.y + 0.1) * (offset.y + 0.1) * (offset.y + 0.1)));
            // LOG("offset " << offset.x << " " << offset.y);
        }
    }
    float length = std::hypot(gradient.x, gradient.y);
    return (length > 0) ? gradient / length : sf::Vector2f(0, 0);
}

float GreenCell::consumeO2(Field &O2_field, float dt)
{
    const sf::Vector2f center = getPosition() + sf::Vector2f(shape.getRadius(), shape.getRadius());
    float total_consumed = 0.0f;
    const float request = consumption_rate * dt;

    // Обход ячеек в радиусе потребления
    for (int dx = -consumption_radius; dx <= consumption_radius; ++dx)
    {
        for (int dy = -consumption_radius; dy <= consumption_radius; ++dy)
        {
            const sf::Vector2f target = center + sf::Vector2f(dx, dy);
            // LOG("target : " << target.x << " " << target.y);
            total_consumed += O2_field.consumeValueAt(target, request);
        }
    }
    return total_consumed;
}