/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** fcntl
*/

#pragma once

#include <fcntl.h>
#include <stdexcept>

namespace Plazza {
    class Fcntl {
        public:
            static void set_NonBlocking(int fd);
    };
}