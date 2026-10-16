/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** fcntl
*/

#include "fcntl.hpp"

namespace Plazza {
    void Fcntl::set_NonBlocking(int fd)
    {
        if (::fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
            throw std::runtime_error("Failed to set non blocking mode");
        }
    }
}