/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** ThreadPool
*/

#include "ThreadPool.hpp"
#include "IPizza.hpp"
#include <memory>
#include <chrono>

namespace Plazza {

    bool ThreadPool::hasEnoughIngredients(const ingredients &stock, const ingredients &needed)
    {
        return stock.dough >= needed.dough
            && stock.tomato >= needed.tomato
            && stock.gruyere >= needed.gruyere
            && stock.ham >= needed.ham
            && stock.mushroom >= needed.mushroom
            && stock.steak >= needed.steak
            && stock.eggplant >= needed.eggplant
            && stock.goatCheese >= needed.goatCheese;
    }

    void ThreadPool::consumeIngredients(ingredients &stock, const ingredients &needed)
    {
        stock.dough -= needed.dough;
        stock.tomato -= needed.tomato;
        stock.gruyere -= needed.gruyere;
        stock.ham -= needed.ham;
        stock.mushroom -= needed.mushroom;
        stock.steak -= needed.steak;
        stock.eggplant -= needed.eggplant;
        stock.goatCheese -= needed.goatCheese;
    }

    ThreadPool::ThreadPool(int nCooks, float timeMultiplier, std::queue<std::shared_ptr<Plazza::IPizza>> &doneQueue,
        std::mutex &doneMutex, int kitchenId, ingredients &ingredients, std::mutex &ingredientsMutex)
        : _currentlyCookingPizzas(0), _stop(false),
            _timeMultiplier(timeMultiplier), _isCooking(nCooks, false), _cookStates(nCooks, FREE),
            _chiefKissTokens(nCooks, 0), _doneQueue(doneQueue), _doneMutex(doneMutex), _kitchenId(kitchenId),
            _ingredients(ingredients), _ingredientsMutex(ingredientsMutex)
    {
        for (int i = 0; i < nCooks; i++)
            _threads.emplace_back(&ThreadPool::cookWorkLoop, this, i + 1, _kitchenId);
    }

    ThreadPool::~ThreadPool()
    {
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _stop = true;
        }
        Plazza::ConditionVariable::notify_all(_conditionVariable);
        for (auto &thread : _threads) {
            if (Plazza::Thread::joinable(thread))
                Plazza::Thread::join(thread);
        }
    }

    void ThreadPool::cookWorkLoop(int cookId, int kitchenId)
    {
        Cook cook(cookId, _timeMultiplier);
        const int cookIndex = cookId - 1;
        
        while (true) {
            std::unique_lock<std::mutex> lock(_mutex);
            Plazza::ConditionVariable::wait(_conditionVariable, lock, [this]() {
                return !_pizzasToCookQueue.empty() || _stop;
            });
            if (_stop && _pizzasToCookQueue.empty())
                return;
            std::shared_ptr<Plazza::IPizza> pizza = _pizzasToCookQueue.front();
            _pizzasToCookQueue.pop();
            _currentlyCookingPizzas++;
            if (cookIndex >= 0 && cookIndex < static_cast<int>(_isCooking.size())) {
                _isCooking[cookIndex] = true;
                _cookStates[cookIndex] = COOKING;
            }
            lock.unlock();

            const ingredients neededIngredients = pizza->getNeededIngredients();
            bool waitingForRefill = false;
            while (true) {
                {
                    std::lock_guard<std::mutex> ingredientsLock(_ingredientsMutex);
                    if (hasEnoughIngredients(_ingredients, neededIngredients)) {
                        consumeIngredients(_ingredients, neededIngredients);
                        break;
                    }
                }
                if (!waitingForRefill) {
                    setCookState(cookIndex, WAITING_FOR_REFILL);
                    waitingForRefill = true;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            setCookState(cookIndex, COOKING);

            cook.makePizza(pizza, kitchenId);

            if (pizza->getType() == Fantasia)
                incrementChiefKissToken(cookIndex);

            {
                std::lock_guard<std::mutex> lock(_doneMutex);
                _doneQueue.push(pizza);
            }
            {
                std::lock_guard<std::mutex> lock(_mutex);
                _currentlyCookingPizzas--;
                if (cookIndex >= 0 && cookIndex < static_cast<int>(_isCooking.size())) {
                    _isCooking[cookIndex] = false;
                    _cookStates[cookIndex] = FREE;
                }
            }
        }
    }

    void ThreadPool::setCookState(int cookIndex, CookState state)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (cookIndex < 0 || cookIndex >= static_cast<int>(_cookStates.size()))
            return;
        _cookStates[cookIndex] = state;
    }

    void ThreadPool::incrementChiefKissToken(int cookIndex)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (cookIndex < 0 || cookIndex >= static_cast<int>(_chiefKissTokens.size()))
            return;
        _chiefKissTokens[cookIndex]++;
    }

    bool ThreadPool::isCooking(int cookIndex)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (cookIndex < 0 || cookIndex >= static_cast<int>(_isCooking.size()))
            return false;
        return _isCooking[cookIndex];
    }

    CookState ThreadPool::getCookState(int cookIndex)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (cookIndex < 0 || cookIndex >= static_cast<int>(_cookStates.size()))
            return FREE;
        return _cookStates[cookIndex];
    }

    int ThreadPool::getCookChiefKissToken(int cookIndex)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (cookIndex < 0 || cookIndex >= static_cast<int>(_chiefKissTokens.size()))
            return 0;
        return _chiefKissTokens[cookIndex];
    }

    void ThreadPool::assignPizza(std::shared_ptr<Plazza::IPizza> pizza)
    {
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _pizzasToCookQueue.push(pizza);
        }
        Plazza::ConditionVariable::notify_one(_conditionVariable);
    }

    int ThreadPool::getPendingPizzaCount()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return static_cast<int>(_pizzasToCookQueue.size()) + _currentlyCookingPizzas;
    }

}

