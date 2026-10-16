/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** Kitchen
*/

#pragma once
#include "Cook.hpp"
#include <vector>
#include <thread>
#include <memory>
#include <queue>
#include <condition_variable>
#include <map>
#include "ThreadPool.hpp"
#include "Ingredients.hpp"
#include "condition_variable.hpp"
#include "thread.hpp"

namespace Plazza {
    struct Pizzeria {
        float timeMultiplier;
        int cookNbPerKitchen;
        int timeToRefill;
    };

    class Kitchen {
        public:
            Kitchen(float timeMultiplier, int cookNbPerKitchen, int timeToRefill, int kitchenId);
            ~Kitchen();
            void refillFoodStock();
            void addPizzaToQueue(std::shared_ptr<Plazza::IPizza> pizza) {_threadPool.assignPizza(pizza); }
            int getCookCount() const { return _cookNbPerKitchen; }
            void collectFinishedPizzas(std::map<int, size_t> &cookedPizzaCount, std::mutex &ordersMutex);
            ThreadPool& getThreadPool() { return _threadPool; }
            ingredients getCurrentIngredients();
            void limitIngredientRefill(int& ingredient);

        private:
            int _cookNbPerKitchen;
            int _timeToRefill;
            int _kitchenId;
            std::mutex _ingredientsMutex;
            ingredients _ingredients;
            
            std::queue<std::shared_ptr<Plazza::IPizza>> _doneQueue;
            std::mutex _doneMutex;

            Plazza::ThreadPool _threadPool;
            
            std::thread _refillThread;
            bool _stopRefill = false;
            std::mutex _refillMutex;
            std::condition_variable _refillConditionVariable;
    };

}
