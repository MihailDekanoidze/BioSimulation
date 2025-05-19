// File: body.hpp
#pragma once
#include "Parameters.hpp"
#include <SFML/Graphics.hpp>
#include "Field.hpp"
#include <fstream>
#include <cmath>

const float sensorRadius = 120.0f;

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
        Field& N2_field,
        std::vector<std::shared_ptr<Body>>& bodies
    ) = 0; // Добавлены параметры полей
    virtual void draw(sf::RenderWindow &window) = 0;
    sf::Vector2f &getPosition();
    sf::Vector2f &getVelocity();
    void setVelocity(const sf::Vector2f& vel){
        velocity = vel;
    }
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
    float max_force;
    float energy;
    float shell;
    sf::CircleShape shape;
    sf::Color basic_color;
    std::vector<sf::Vector2f> checkPoints; // Относительные координаты точек проверки
    int step = 5;                          // Шаг между точками
    Field_types decay_field;
    sf::Vector2f direction;     // Текущее случайное направление

    float enegry_to_mass;
    float div_mass;
    sf::Vector2f force; 

    float divide_energy_threshold = 99.0f; // Минимальная энергия для деления
    float divide_cooldown = 5.0f; // Время между делениями
    bool is_divided;
    float initial_div_time = 10.0f;
    float divide_timer = 0.0f;
public:
    // Конструкторы
    Cell(const sf::Vector2f& pos, const sf::Vector2f& vel, const sf::Vector2f& acr, float mass,
        float radius, float mfr, float eng, float shl, const sf::Color& color, Field_types ft, float em, float dm, 
        const sf::Vector2f& rd, const sf::Vector2f& fr)
        : Body({pos.x - radius, pos.y - radius}, vel, acr, mass),
          max_force(mfr),
          energy(eng),
          shell(shl),
          basic_color(color),
          decay_field(ft),
          enegry_to_mass(em),
          div_mass(dm),
          force(fr),
          is_divided(false)
    {
        shape.setRadius(radius);
        shape.setPosition(pos);
        shape.setFillColor(basic_color);
        initializeCheckPoints(sensorRadius);
        this->setDir(rd);
    }

    // Копирующий конструктор
    Cell(const Cell& other)
        : Body(other), // Копирование базового класса
          max_force(other.max_force),
          energy(other.energy),
          shell(other.shell),
          shape(other.shape), // Копирование графического представления
          checkPoints(other.checkPoints),
          step(other.step),
          basic_color(other.basic_color), // Копирование цвета
          decay_field(other.decay_field),
          direction(other.direction),
          enegry_to_mass(other.enegry_to_mass),
          div_mass(other.div_mass),
          force(other.force),
          is_divided(false)
    {       
        // Логирование или дополнительные действия при копировании
        LOG("Cell copy constructor");
    }
    void draw(sf::RenderWindow &window) override;

    // Инициализация точек проверки в квадрате с шагом 2
    void initializeCheckPoints(float sensorRadius);
    void addForce(const sf::Vector2f& f){
        force += f;
    }
    virtual ~Cell(){}
    

    // Методы доступа
    sf::Vector2f getForce()const{
        return force;
    }
    void setForce(const sf::Vector2f& fr){
        force = fr;
    }
    float getRaduis() const{
        return shape.getRadius();
    }
    Field_types getDecay() const{
        return decay_field;
    }
    float getMaxforce() const
    {
        return max_force;
    }
    float getEnergy() const
    {
        return energy;
    }
    float getShell() const
    {
        return shell;
    }
    void setDir(const sf::Vector2f dir);
    sf::Vector2f getDir()const{
        return direction;
    }
    void setMaxforce(float mfr)
    {
        max_force = mfr;
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
    void setOwnForce();
    // Обновление с учетом новых свойств
    virtual bool update(float dt, Field& O2, Field& H2, Field& N2, Field& C, std::vector<std::shared_ptr<Body>>& bodies){return true;};
    bool updateBasic(float dt, std::vector<std::shared_ptr<Body>>& bodies);
    void setPosition(const sf::Vector2f& pos){
        position = pos;
        shape.setPosition(pos);  
    }
    virtual bool tryDivide(std::vector<std::shared_ptr<Body>>& bodies) = 0;
    
    //void Cell::updatePosterity(void);

    void info(void) override;
    static bool isPositionFree(const sf::Vector2f& pos, float radius, 
        const std::vector<std::shared_ptr<Body>>& bodies);
};

