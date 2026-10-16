/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** write
*/

#include "write.hpp"
#include <stdexcept>
#include <cerrno>

namespace Plazza {
    ssize_t Write::Write_data(int fd, const std::string &buffer)
    {
        ssize_t bytesWritten = ::write(fd, buffer.data(), buffer.size());
        if (bytesWritten == -1) {
            if (errno == EPIPE)
                return 0;
            throw std::runtime_error("Failed to write");
        }
        return bytesWritten;
    }

    ssize_t Write::Write_data(int fd, const std::vector<char> &buffer)
    {
        ssize_t bytesWritten = ::write(fd, buffer.data(), buffer.size());
        if (bytesWritten == -1) {
            if (errno == EPIPE)
                return 0;
            throw std::runtime_error("Failed to write");
        }
        return bytesWritten;
    }
}
