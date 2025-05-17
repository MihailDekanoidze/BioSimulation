// File: body.hpp
#pragma once
#include "Parameters.hpp"
#include <SFML/Graphics.hpp>
#include "Field.hpp"
#include <fstream>

const float sensorRadius = 1200.0f;

class Body : virtual protected Log
{
protected:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    float mass;
public:
    Body() : position(sf::Vector2f(0, 0)), velocity(sf::Vector2f(0, 0)), mass(0), Log(){}
    Body(const sf::Vector2f &pos, const sf::Vector2f &vel, const sf::Vector2f& acr, float mass):position(pos), velocity(vel), acceleration(acr), mass(mass), Log(){}
    virtual bool update(
        float dt, 
        Field& O2_field, 
        Field& H2_field, 
        Field& C_field, 
        Field& N2_field
    ) = 0; // Добавлены параметры полей
    virtual void draw(sf::RenderWindow &window) = 0;
    sf::Vector2f &getPosition();
    sf::Vector2f &getVelocity();
    float getMass() const;
    virtual void info(void){}
    Body(const Body &other) : position(other.position), velocity(other.velocity), acceleration(other.acceleration), mass(other.mass), Log()
    {
        LOG("Copied Body object");
        // Дополнительные действия при копировании
    }
    virtual ~Body()=default;
};

class Cell : public Body
{
protected:
    float speed;
    float energy;
    float shell;
    sf::CircleShape shape;
    sf::Color basic_color;
    std::vector<sf::Vector2f> checkPoints; // Относительные координаты точек проверки
    int step = 5;                          // Шаг между точками
    Field_types decay_field;
    sf::Vector2f random_direction;     // Текущее случайное направление
    float enegry_to_mass;
    float div_mass;
    sf::Vector2f force; 
public:
    // Конструкторы
    Cell(const sf::Vector2f& pos, const sf::Vector2f& vel, const sf::Vector2f& acr, float mass,
        float radius, float spd, float eng, float shl, const sf::Color& color, Field_types ft, float em, float dm, 
        const sf::Vector2f& rd, const sf::Vector2f& fr)
        : Body({pos.x - radius, pos.y - radius}, vel, acr, mass),
          speed(spd),
          energy(eng),
          shell(shl),
          basic_color(color),
          decay_field(ft),
          random_direction(rd),
          enegry_to_mass(em),
          div_mass(dm),
          force(fr)
    {
        shape.setRadius(radius);
        shape.setPosition(pos);
        shape.setFillColor(basic_color);
        initializeCheckPoints(sensorRadius);

    }

    // Копирующий конструктор
    Cell(const Cell& other)
        : Body(other), // Копирование базового класса
          speed(other.speed),
          energy(other.energy),
          shell(other.shell),
          shape(other.shape), // Копирование графического представления
          checkPoints(other.checkPoints),
          step(other.step),
          basic_color(other.basic_color), // Копирование цвета
          decay_field(other.decay_field),
          random_direction(other.random_direction),
          enegry_to_mass(other.enegry_to_mass),
          div_mass(other.div_mass),
          force(other.force)
    {       
        // Логирование или дополнительные действия при копировании
        LOG("Cell copy constructor");
    }
    void draw(sf::RenderWindow &window) override;

    // Инициализация точек проверки в квадрате с шагом 2
    void initializeCheckPoints(float sensorRadius);
    void applyForce(const sf::Vector2f& f){
        force = f;
    }
    virtual ~Cell(){}
    

    // Методы доступа
    float getRaduis() const{
        return shape.getRadius();
    }
    Field_types getDecay() const{
        return decay_field;
    }
    float getSpeed() const
    {
        return speed;
    }
    float getEnergy() const
    {
        return energy;
    }
    float getShell() const
    {
        return shell;
    }
    void setSpeed(float spd)
    {
        speed = spd;
    }
    void setEnergy(float eng)
    {
        energy = eng;
    }
    void setShell(float shl)
    {
        shell = shl;
    }

    void updateColor();
    void setTargetVelocity(const sf::Vector2f&);
    // Обновление с учетом новых свойств
    bool update(float dt, Field& O2, Field& H2, Field& N2, Field& C) = 0;
    void updateBasic(float dt);
    void setPosition(const sf::Vector2f& pos){
        position = pos;
        shape.setPosition(pos);  
    }
    //void Cell::updatePosterity(void);

    void info(void) override;
};

