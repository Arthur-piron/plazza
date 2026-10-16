/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** condition_variable
*/

#include "condition_variable.hpp"

namespace Plazza {
    void ConditionVariable::wait(std::condition_variable &conditionalVariable, std::unique_lock<std::mutex> &lock)
    {
        try {
            conditionalVariable.wait(lock);
        } catch (const std::exception &e) {
            throw std::runtime_error("Failed to wait on condition variable");
        }
    }

    void ConditionVariable::notify_one(std::condition_variable &conditionalVariable)
    {
        try {
            conditionalVariable.notify_one();
        } catch (const std::exception &e) {
            throw std::runtime_error("Failed to notify one on condition variable");
        }
    }

    void ConditionVariable::notify_all(std::condition_variable &conditionalVariable)
    {
        try {
            conditionalVariable.notify_all();
        } catch (const std::exception &e) {
            throw std::runtime_error("Failed to notify all on condition variable");
        }
    }
}
