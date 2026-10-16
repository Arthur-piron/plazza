/*
** EPITECH PROJECT, 2026
** Graphical
*/

#include "Graphical.hpp"

namespace Plazza {

Graphical::Graphical(Plazza::Pizzeria &pizzeria, Receptionist &receptionist)
    : _pizzeria(pizzeria), _receptionist(receptionist)
{
}

int Graphical::run()
{
    sf::RenderWindow window(
        sf::VideoMode(1920, 1080),
        "Plazza",
        sf::Style::Titlebar | sf::Style::Close
    );
    window.setFramerateLimit(60);

    Renderer renderer(window, _receptionist);
    renderer.setOrderCallback([&](const std::string &line) {
        _receptionist.processOrder(line, _pizzeria, renderer);
    });
    _receptionist.startWatcher(renderer);

    sf::Clock clock;
    while (window.isOpen() && !_receptionist.shouldExit()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed
                || (event.type == sf::Event::KeyPressed
                    && event.key.code == sf::Keyboard::Escape)) {
                renderer._audioManager.stopAll();
                window.close();
            }
            renderer.handleEvent(event);
        }
        if (!window.isOpen())
            break;

        float deltaTime = clock.restart().asSeconds();
        renderer.update(deltaTime);
        window.clear();
        renderer.draw();
        window.display();
    }
    _receptionist.stopWatcher();
    _receptionist.stopAllKitchens(true);
    return 0;
}

}
