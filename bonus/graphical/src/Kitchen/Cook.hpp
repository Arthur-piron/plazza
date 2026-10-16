/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** Cook
*/

#pragma once

#include "PizzaHandler.hpp"
#include <memory>
#include <condition_variable>
#include <queue>
#include "Ingredients.hpp"
    
namespace Plazza {
    enum CookState { FREE, COOKING, WAITING_FOR_REFILL };

    class ThreadPool;
    class Cook {
        public:
            Cook(int id, float timeMultiplier);
            ~Cook();
            void makePizza(std::shared_ptr<Plazza::IPizza> pizza, int kitchenId);
        private:
            int _id;
            float _timeMultiplier;
};
}
