// Файл: GreenCell.hpp
#pragma once
#include "Cell.hpp"
#include <random>

class GreenCell : public Cell {
private:
    const float enegry_consume = 1.0f;
    // Параметры случайного движения
    float random_move_duration = 4.0f; // Длительность случайного движения (сек)
    float random_move_timer = 0.0f;    // Таймер
    
    float energy_conversion = 0.7f;      // Коэффициент преобразования O2 -> energy
    float consumption_radius;    // Радиус потребления
    float consumption_rate = 0.5f;       // Единиц O2/сек
    std::mt19937 rng{std::random_device{}()};

public:
    GreenCell(const sf::Vector2f& pos, const sf::Vector2f& vel, const sf::Vector2f& acr, float mass,
             float radius, float spd, float eng, float shl,  float em, float dm, const sf::Vector2f& fr, sf::Vector2f rd = {1, 0})
        : Cell(pos, vel, acr, mass, radius, spd, eng, shl, sf::Color::Green, O2, em, dm, rd, fr)
    {
        generateNewRandomDirection(); 
        consumption_radius = radius;
    }

    GreenCell(const GreenCell& other)
        : Cell(other),
          random_move_duration(other.random_move_duration),
          random_move_timer(other.random_move_timer),
          consumption_radius(other.consumption_radius),
          rng(other.rng) 
    {
    }
    bool update(float dt, Field& O2_field, Field& H2_field,
                Field& C_field, Field& N2_field) override;

private:
    void generateNewRandomDirection();
    sf::Vector2f calculateO2Gradient(const Field& O2_field);
    float consumeO2(Field& O2_field, float dt);
};