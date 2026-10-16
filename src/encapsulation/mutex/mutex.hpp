/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** mutex
*/

#pragma once

#include <mutex>
#include <stdexcept>

namespace Plazza {
    class Mutex {
        public:
            static void lock(std::mutex &mutex);
            static void unlock(std::mutex &mutex);
            static bool try_lock(std::mutex &mutex);
    };
}
