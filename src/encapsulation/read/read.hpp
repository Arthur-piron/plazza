/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** read
*/

#pragma once

#include <string>
#include <vector>
#include <unistd.h>
#include <cerrno>

namespace Plazza {
    class Read_data {
        public:
            static ssize_t read_data(int fd, std::string &buffer);
            static ssize_t read_data(int fd, std::vector<char> &buffer);
    };
}
