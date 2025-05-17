#pragma once
#include "Parameters.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <vector>
#include <memory>

enum Field_types{
    O2 = 0,
    H2 = 1,
    N2 = 3,
    C  = 4,
};


class Field : virtual protected Log
{
private:
    struct PixelData
    {
        float value;          // 0-100+: 0 - черный, 100 - полная яркость базового цвета
        sf::Color base_color; // Цвет при значении 100
    };

    std::vector<std::vector<PixelData>> grid;
    sf::RenderTexture field_texture;
    sf::Sprite field_sprite;
    unsigned width;
    unsigned height;

public:
    // Конструктор для равномерного заполнения
    Field(unsigned w, unsigned h, float init_value, const sf::Color &base_color)
        : grid(w, std::vector<PixelData>(h)), width(w), height(h)
    {
        field_texture.create(width, height);

        // Инициализация всех пикселей
        for (auto &row : grid)
        {
            for (auto &pixel : row)
            {
                pixel.value = init_value;
                pixel.base_color = base_color;
            }
        }
    }

    // Проверка, находится ли точка в пределах поля
    bool isInside(const sf::Vector2f& pos) const;
    float getValue(float x, float  y) const;

    float getValue(const sf::Vector2f& pos) const;
    // Метод рендеринга с учетом яркости
    void render(sf::RenderWindow &window);

    // Метод для изменения значений в области круга
    void applyCircularArea(const sf::CircleShape &area, float value, bool additive = false);

    // Метод для изменения базового цвета
    void setBaseColor(const sf::Color &new_color);

    // Метод для равномерного изменения значений
    void setUniformValue(float new_value);

    sf::Vector2f findLocalMax(const sf::Vector2f& center, float radius) const;
        // Уменьшает значение в позиции (x,y) на указанную величину. Возвращает фактически потребленное значение
    float consumeValueAt(const sf::Vector2f& pos, float amount);
};
