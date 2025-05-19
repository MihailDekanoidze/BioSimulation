#include "../include/Field.hpp"
#include <random>
#include <vector>

void Field::render(sf::RenderWindow &window)
{
    field_texture.clear();

    sf::Image field_image;
    field_image.create(width, height);

    for (unsigned x = 0; x < width; ++x)
    {
        for (unsigned y = 0; y < height; ++y)
        {
            const auto &pixel = grid[x][y];

            // Рассчитываем яркость (0.0-1.0)
            float brightness = std::clamp(pixel.value, 0.0f, 1.0f);

            // Применяем яркость к базовому цвету
            sf::Color final_color = base_color;
            final_color.r = static_cast<sf::Uint8>(final_color.r * brightness);
            final_color.g = static_cast<sf::Uint8>(final_color.g * brightness);
            final_color.b = static_cast<sf::Uint8>(final_color.b * brightness);

            field_image.setPixel(x, y, final_color);
        }
    }

    sf::Texture tex;
    tex.loadFromImage(field_image);
    field_sprite.setTexture(tex);
    field_texture.draw(field_sprite);
    field_texture.display();

    window.draw(sf::Sprite(field_texture.getTexture()));
}

void Field::applyCircularArea(const sf::CircleShape &area, float value, bool additive)
{
    const sf::Vector2f center = area.getPosition();
    const float radius = area.getRadius();
    for (unsigned x = 0; x < width; ++x)
    {
        for (unsigned y = 0; y < height; ++y)
        {
            sf::Vector2f pixel_pos(x, y);
            sf::Vector2f delta = pixel_pos - center;
            float distance = std::hypot(delta.x, delta.y);
            if (distance <= radius)
            {
                if (additive)
                {
                    grid[x][y].value += value;
                }
                else
                {
                    grid[x][y].value = value;
                }
                // Ограничение минимального значения
                grid[x][y].value = std::max(grid[x][y].value, 0.0f);
            }
        }
    }
}

void Field::setBaseColor(const sf::Color &new_color)
{
    base_color = new_color;
}

void Field::setUniformValue(float new_value)
{
    for (auto &row : grid)
    {
        for (auto &pixel : row)
        {
            pixel.value = new_value;
        }
    }
}

sf::Vector2f Field::findLocalMax(const sf::Vector2f &center, float radius) const
{
    float max_value = -std::numeric_limits<float>::infinity();
    sf::Vector2f max_pos = center;

    // Определяем границы поиска (целочисленные индексы)
    const int min_x = static_cast<int>(std::floor(center.x - radius));
    const int max_x = static_cast<int>(std::ceil(center.x + radius));
    const int min_y = static_cast<int>(std::floor(center.y - radius));
    const int max_y = static_cast<int>(std::ceil(center.y + radius));

    // Ограничиваем границы размерами сетки
    const int clamped_min_x = std::clamp(min_x, 0, static_cast<int>(width) - 1);
    const int clamped_max_x = std::clamp(max_x, 0, static_cast<int>(width) - 1);
    const int clamped_min_y = std::clamp(min_y, 0, static_cast<int>(height) - 1);
    const int clamped_max_y = std::clamp(max_y, 0, static_cast<int>(height) - 1);

    // Перебор всех ячеек в ограниченной области
    for (int x = clamped_min_x; x <= clamped_max_x; ++x)
    {
        for (int y = clamped_min_y; y <= clamped_max_y; ++y)
        {
            // Проверка расстояния от центра до ячейки
            const sf::Vector2f cell_center(x + 0.5f, y + 0.5f);
            const float distance = std::hypot(cell_center.x - center.x, cell_center.y - center.y);

            if (distance <= radius && grid[x][y].value > max_value)
            {
                max_value = grid[x][y].value;
                max_pos = cell_center;
            }
        }
    }

    return max_pos;
}

float Field::consumeValueAt(const sf::Vector2f &pos, float amount)
{
    int x = static_cast<int>(pos.x);
    int y = static_cast<int>(pos.y);
    if (x < 0 || x >= width || y < 0 || y >= height)
        return 0.0f;

    float consumed = std::min(grid[x][y].value, amount);
    grid[x][y].value -= consumed;

    return consumed;
}

bool Field::isInside(const sf::Vector2f &pos) const
{
    return (pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height);
}

float Field::getValue(float x, float y) const
{
    if (x < 0 || static_cast<unsigned>(x) > width || y < 0 || static_cast<unsigned>(y) > height)
        return 0;
    return grid[x][y].value;
}

float Field::getValue(const sf::Vector2f &pos) const
{
    if (pos.x < 0 || pos.y < 0 || static_cast<unsigned>(pos.x) > width || static_cast<unsigned>(pos.y) >= height)
        return 0;
    return grid[static_cast<unsigned>(pos.x)][static_cast<unsigned>(pos.y)].value;
}

void Field::setRandomPattern(void)
{
    std::uniform_int_distribution<int> dist2(5, 20);
    size_t count = dist2(rng);
    std::uniform_real_distribution<float> dist1(pattern_radius + y_offset, height - pattern_radius - y_offset);
    for (size_t i = 0; i < count; i++)
    {
        sf::CircleShape area = sf::CircleShape(pattern_radius);
        area.setPosition((width - height) / 2 + dist1(rng), dist1(rng));
        this->applyCircularArea(area, default_pattern_value);
    }
}