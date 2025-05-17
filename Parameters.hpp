#pragma once
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>

#define LOG(string) log_file << string << std::endl; \
        log_file.flush()

const unsigned int width = 1920;
const unsigned int height = 1080;
const unsigned int bitsperpixel = 64U;
const unsigned int x_offset = 50;
const unsigned int y_offset = 100;

const float capacity = 100.0f;
const float epsilon = 0.00001f;

const sf::Color H2_color = sf::Color(255, 102, 178);
const sf::Color N2_color = sf::Color(0, 255, 128);
const sf::Color O2_color = sf::Color(153, 255, 255);
const sf::Color C_color = sf::Color(192, 192, 192);

const sf::Vector2f down_gravity(.0f, 98.f);
const sf::Vector2f up_gravity(.0f, -98.f);
const sf::Vector2f right_gravity(98.f, .0f);
const sf::Vector2f left_gravity(-98.f, .0f);

class Log
{
protected:
    std::ofstream log_file;
    public:
    Log() {
        log_file.open("log.txt", std::ios::out | std::ios::trunc); // Открываем файл при создании
        if (!log_file.is_open()) {
            throw std::runtime_error("Cannot open log file");
        }
    }

    virtual ~Log() {
        log_file.close(); // Закрываем файл при разрушении
    }
};

