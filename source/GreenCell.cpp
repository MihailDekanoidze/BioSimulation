#include "../include/GreenCell.hpp"

bool GreenCell::update(float dt, Field &O2_field, Field &H2_field, Field &C_field, Field &N2_field,
                       std::vector<std::shared_ptr<Body>> &bodies)
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
            LOG("Generated dir " << direction.x << " " << direction.y);
        }
        random_move_timer -= dt;
        LOG("Rand timer" << random_move_timer);
    }
    else
    {
        LOG("Gradient direction " << grad.x << " " << grad.y);
        setDir(grad);
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
    if (this->updateBasic(dt, bodies))
        random_move_timer = random_move_duration;
    return true;
}

void GreenCell::generateNewRandomDirection()
{
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    setDir(sf::Vector2f(dist(rng), dist(rng)));
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
            if (!(target.x < 0 || target.x >= width || target.y < 0 || target.y >= height))
                total_consumed += O2_field.consumeValueAt(target, request);
        }
    }
    return total_consumed;
}

bool GreenCell::tryDivide(std::vector<std::shared_ptr<Body>> &bodies)
{
    if (energy < divide_energy_threshold)
        return false;

    // Генерация направления деления
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angle_dist(0.0f, 2.0f * 3.14159f);
    float angle = angle_dist(gen);
    sf::Vector2f dir(std::cos(angle), std::sin(angle));
    sf::Vector2f new_pos = position + dir * (getRaduis() * 2.5f);

    // Проверка позиции
    if (!isPositionFree(new_pos, getRaduis(), bodies))
        return false;

    // Создаем новую клетку как GreenCell
    GreenCell *new_green_cell = new GreenCell(*this); // Используем копирующий конструктор GreenCell
    new_green_cell->is_divided = false;
    new_green_cell->setPosition(new_pos);
    new_green_cell->setEnergy(energy * 0.5f);
    new_green_cell->setVelocity({1, 0}); // Начальная скорость

    this->energy *= 0.5f;
    bodies.push_back(std::shared_ptr<Body>(new_green_cell));
    return true;
}