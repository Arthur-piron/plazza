/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** mutex
*/

#include "mutex.hpp"

namespace Plazza {
    void Mutex::lock(std::mutex &mutex)
    {
        try {
            mutex.lock();
        } catch (const std::exception &e) {
            throw std::runtime_error("Failed to lock mutex");
        }
    }

    void Mutex::unlock(std::mutex &mutex)
    {
        try {
            mutex.unlock();
        } catch (const std::exception &e) {
            throw std::runtime_error("Failed to unlock mutex");
        }
    }

    bool Mutex::try_lock(std::mutex &mutex)
    {
        try {
            return mutex.try_lock();
        } catch (const std::exception &e) {
            throw std::runtime_error("Failed to try lock mutex");
        }
    }
}
