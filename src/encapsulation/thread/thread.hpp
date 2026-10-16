/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** thread
*/

#pragma once

#include <thread>
#include <stdexcept>

namespace Plazza {
    class Thread {
        public:
            static void join(std::thread &thread);
            static void detach(std::thread &thread);
            static bool joinable(std::thread &thread);
    };
}
