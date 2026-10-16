/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** ThreadPool
*/

#ifndef THREADPOOL_HPP_
#define THREADPOOL_HPP_

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "IPizza.hpp"
#include "PizzaHandler.hpp"
#include "Cook.hpp"
#include "Ingredients.hpp"
#include "condition_variable.hpp"
#include "mutex.hpp"
#include "thread.hpp"


namespace Plazza {

    class ThreadPool {
    public:
        ThreadPool(int nCooks, float timeMultiplier,
            std::queue<std::shared_ptr<Plazza::IPizza>> &doneQueue,
            std::mutex &doneMutex, int kitchenId,
            ingredients &ingredients, std::mutex &ingredientsMutex);
        ~ThreadPool();
        void assignPizza(std::shared_ptr<Plazza::IPizza> pizza);
        void cookWorkLoop(int cookId, int kitchenId);
        bool isCooking(int cookIndex);
        int getPendingPizzaCount();
        CookState getCookState(int cookIndex);
        int getCookChiefKissToken(int cookIndex);

        std::queue<std::shared_ptr<Plazza::IPizza>>& getDoneQueue() { return _doneQueue; }
        std::mutex& getDoneMutex() { return _doneMutex; }

        bool hasEnoughIngredients(const ingredients &stock, const ingredients &needed);
        void consumeIngredients(ingredients &stock, const ingredients &needed);

    private:
        void setCookState(int cookIndex, CookState state);
        void incrementChiefKissToken(int cookIndex);

        std::vector<std::thread> _threads;
        std::queue<std::shared_ptr<Plazza::IPizza>> _pizzasToCookQueue;
        std::mutex _mutex;
        std::condition_variable  _conditionVariable;
        int _currentlyCookingPizzas = 0;
        bool _stop;

        float _timeMultiplier;
        std::vector<bool> _isCooking;
        std::vector<CookState> _cookStates;
        std::vector<int> _chiefKissTokens;
        std::queue<std::shared_ptr<Plazza::IPizza>> &_doneQueue;
        std::mutex &_doneMutex;
        int _kitchenId;
        ingredients &_ingredients;
        std::mutex &_ingredientsMutex;
    };
}

#endif /* !THREADPOOL_HPP_ */
