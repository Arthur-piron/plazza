/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** thread
*/

#include "thread.hpp"

namespace Plazza {
    void Thread::join(std::thread &thread)
    {
        try {
            if (thread.joinable())
                thread.join();
        } catch (const std::exception &e) {
            throw std::runtime_error("Failed to join thread");
        }
    }

    void Thread::detach(std::thread &thread)
    {
        try {
            if (thread.joinable())
                thread.detach();
        } catch (const std::exception &e) {
            throw std::runtime_error("Failed to detach thread");
        }
    }

    bool Thread::joinable(std::thread &thread)
    {
        try {
            return thread.joinable();
        } catch (const std::exception &e) {
            throw std::runtime_error("Failed to check if thread is joinable");
        }
    }
}
