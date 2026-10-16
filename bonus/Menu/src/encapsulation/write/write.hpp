/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** write
*/

#pragma once

#include <string>
#include <vector>
#include <unistd.h>

namespace Plazza {
    class Write {
        public:
            static ssize_t Write_data(int fd, const std::string &buffer);
            static ssize_t Write_data(int fd, const std::vector<char> &buffer);
    };
}
