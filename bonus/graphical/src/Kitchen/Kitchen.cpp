/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** Kitchen
*/

#include "Kitchen.hpp"
#include <iostream>
#include "Utilities.hpp"
#include <chrono>
#include <climits>
#include "condition_variable.hpp"

namespace Plazza {

Kitchen::Kitchen(float timeMultiplier, int cookNbPerKitchen, int timeToRefill, int kitchenId)
    : _cookNbPerKitchen(cookNbPerKitchen),
    _timeToRefill(timeToRefill),
    _kitchenId(kitchenId),
    _threadPool(cookNbPerKitchen, timeMultiplier, _doneQueue, _doneMutex, kitchenId, _ingredients, _ingredientsMutex)
{
    Utilities::writeOutput("Kitchen [#" + std::to_string(_kitchenId) + "] Created");
    _ingredients.dough = 5;
    _ingredients.tomato = 5;
    _ingredients.gruyere = 5;
    _ingredients.ham = 5;
    _ingredients.mushroom = 5;
    _ingredients.steak = 5;
    _ingredients.eggplant = 5;
    _ingredients.goatCheese = 5;

    _refillThread = std::thread(&Kitchen::refillFoodStock, this);
}

Kitchen::~Kitchen()
{
    {
        std::lock_guard<std::mutex> lock(_refillMutex);
        _stopRefill = true;
    }
    Plazza::ConditionVariable::notify_one(_refillConditionVariable);
    if (Plazza::Thread::joinable(_refillThread))
        Plazza::Thread::join(_refillThread);
    Utilities::writeOutput("Kitchen [#" + std::to_string(_kitchenId) + "] Destroyed");
}

void Kitchen::limitIngredientRefill(int& ingredient)
{
    if (ingredient < INT_MAX)
        ingredient += 1;
}

void Kitchen::refillFoodStock()
{
    std::unique_lock<std::mutex> lock(_refillMutex);
    while (!_stopRefill) {
        _refillConditionVariable.wait_for(lock, std::chrono::milliseconds(_timeToRefill), [this] { return _stopRefill; });
        if (_stopRefill)
            break;
        {
            std::lock_guard<std::mutex> lock(_ingredientsMutex);
            Plazza::Utilities::writeOutput("Kitchen " + std::to_string(_kitchenId) + " is refilling ingredients...");
            limitIngredientRefill(_ingredients.dough);
            limitIngredientRefill(_ingredients.tomato);
            limitIngredientRefill(_ingredients.gruyere);
            limitIngredientRefill(_ingredients.ham);
            limitIngredientRefill(_ingredients.eggplant);
            limitIngredientRefill(_ingredients.mushroom);
            limitIngredientRefill(_ingredients.steak);
            limitIngredientRefill(_ingredients.goatCheese);
        }
    }
}

void Kitchen::collectFinishedPizzas(std::map<int, size_t> &cookedPizzaCount, std::mutex &ordersMutex)
{
    std::vector<std::shared_ptr<Plazza::IPizza>> done;
    {
        std::lock_guard<std::mutex> lock(_doneMutex);
        while (!_doneQueue.empty()) {
            done.push_back(_doneQueue.front());
            _doneQueue.pop();
        }
    }

    std::lock_guard<std::mutex> orderLock(ordersMutex);
    for (auto &pizza : done) {
        cookedPizzaCount[pizza->getOrderId()] += 1;
    }
}

ingredients Kitchen::getCurrentIngredients()
{
    std::lock_guard<std::mutex> lock(_ingredientsMutex);
    return _ingredients;
}

}
