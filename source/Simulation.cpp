#include "../include/Simulation.hpp"
#include "../include/Cell.hpp"
#include "../include/Environment.hpp"
#include "../include/Field.hpp"
#include "../include/GreenCell.hpp"
#include "../include/Parameters.hpp"
#include <SFML/Graphics.hpp>

void Simulation::init(void)
{
    sf::CircleShape cs = sf::CircleShape(5);
    cs.setPosition({500, 700});
    env.addCircleO(cs, 0.25, true);
    GreenCell first_cell = GreenCell({150, 150}, {10, 0}, {0, 0}, 1, 50, 50, 50, 1, 0.5, 2, {0, 0});
    env.addGCell(first_cell);
}

void Simulation::pause(const sf::Event::KeyEvent &key)
{
    if (key.code == sf::Keyboard::Space)
    {
        is_pause = !is_pause;
    }
};

void Simulation::drawPauseMenu()
{
    // Рисуем текст "PAUSED"
    sf::Text pauseText("PAUSED", font, 48);
    pauseText.setPosition(width / 2 - 80, height / 2 - 200);
    pauseText.setFillColor(sf::Color::White);
    window.draw(pauseText);

    // Рисуем подсказки
    sf::Text helpText(
        "Press:\n"
        "O - Toggle O2 Field\n"
        "H - Toggle H2 Field\n"
        "C - Toggle C Field\n"
        "N - Toggle N2 Field",
        font, 24);
    helpText.setPosition(50, 50);
    helpText.setFillColor(sf::Color::White);
    window.draw(helpText);

    // Рисуем активные поля
    if (show_O2)
        env.drawOField(window);
    if (show_H2)
        env.drawHField(window);
    if (show_C)
        env.drawCField(window);
    if (show_N2)
        env.drawNField(window);
}

void Simulation::handlePauseMenuInput(const sf::Event::KeyEvent &key)
{
    if (key.code == sf::Keyboard::O)
        show_O2 = !show_O2;
    if (key.code == sf::Keyboard::H)
        show_H2 = !show_H2;
    if (key.code == sf::Keyboard::C)
        show_C = !show_C;
    if (key.code == sf::Keyboard::N)
        show_N2 = !show_N2;
}

void Simulation::run(void)
{
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed)
            {
                pause(event.key);
                if (is_pause)
                    handlePauseMenuInput(event.key);
            }
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    // Получение координат клика
                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;
                    sf::CircleShape cs1 = sf::CircleShape(5);
                    cs1.setPosition({mouseX, mouseY});
                    env.addCircleO(cs1, 0.25, true);
                }
            }
        }

        window.clear(sf::Color::Black);

        if (!is_pause)
        {
            float dt = clock.restart().asSeconds();
            sf::CircleShape control = sf::CircleShape(20);
            control.setFillColor(sf::Color::Red);
            control.setPosition({1000, 200});

            env.updatePhysics(dt);
            window.setView(window.getDefaultView());

            env.drawOField(window);
            env.drawBodies(window);

            window.draw(control);
        }
        else
        {
            drawPauseMenu();
            clock.restart();
        }

        window.display();
    }
}