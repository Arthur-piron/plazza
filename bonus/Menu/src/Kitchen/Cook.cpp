/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** Cook
*/

#include "Cook.hpp"
#include "Utilities.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include <fstream>

namespace Plazza {

    Cook::Cook(int id, float timeMultiplier) 
        : _id(id), _timeMultiplier(timeMultiplier)
    {
    }

    Cook::~Cook()
    {
    }

    void Cook::makePizza(std::shared_ptr<Plazza::IPizza> pizza, int kitchenId)
    {
        int cookingTime = pizza->getCookingTime() * _timeMultiplier;
        std::this_thread::sleep_for(std::chrono::milliseconds(cookingTime));
        pizza->setCooked(true);
        Plazza::PizzaHandler parser;
        Plazza::Utilities::writeOutput("Cook " + std::to_string(_id) +
            " (Kitchen [#" + std::to_string(kitchenId) + "]) has made a " +
            parser.pizzaSizeToString(pizza->getSize()) + " " +
            parser.pizzaTypeToString(pizza->getType()) + " pizza!");
    }

}
