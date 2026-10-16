/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** read
*/

#include "read.hpp"

#include <stdexcept>
#include <cerrno>

namespace Plazza {
    ssize_t Read_data::read_data(int fd, std::string &buffer)
    {
        ssize_t bytesRead = ::read(fd, buffer.data(), buffer.size());
        
        if (bytesRead == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) { // NON BLOCKING
                return -1;
            }
            throw std::runtime_error("Failed to read");
        }
        return bytesRead;
    }

    ssize_t Read_data::read_data(int fd, std::vector<char> &buffer)
    {
        ssize_t bytesRead = ::read(fd, buffer.data(), buffer.size());

        if (bytesRead == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return -1;
            throw std::runtime_error("Failed to read");
        }
        return bytesRead;
    }
}
