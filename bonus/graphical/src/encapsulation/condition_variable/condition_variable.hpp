/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** condition_variable
*/

#pragma once

#include <condition_variable>
#include <mutex>
#include <stdexcept>

namespace Plazza {
    class ConditionVariable {
        public:
            static void wait(std::condition_variable &conditionalVariable, std::unique_lock<std::mutex> &lock);
            static void notify_one(std::condition_variable &conditionalVariable);
            static void notify_all(std::condition_variable &conditionalVariable);
            
            template<typename Predicate>
            static void wait(std::condition_variable &conditionalVariable, std::unique_lock<std::mutex> &lock, Predicate predicate)
            {
                try {
                    conditionalVariable.wait(lock, predicate);
                } catch (const std::exception &e) {
                    throw std::runtime_error("Failed to wait on condition variable");
                }
            }
    };
}

