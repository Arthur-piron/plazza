/*
** EPITECH PROJECT, 2026
** Graphical
*/

#pragma once
#include "Receptionist.hpp"
#include "Renderer.hpp"
#include <SFML/Graphics.hpp>

namespace Plazza {
    class Graphical {
        public:
            Graphical(Plazza::Pizzeria &pizzeria, Receptionist &receptionist);
            ~Graphical() = default;
            int run();

        private:
            Plazza::Pizzeria &_pizzeria;
            Receptionist &_receptionist;
    };
}
