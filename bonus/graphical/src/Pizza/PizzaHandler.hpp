/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** Pizza
*/

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <iostream>
#include "IPizza.hpp"

#include "APizza.hpp"
namespace Plazza {
    class Kitchen;
    struct Pizzeria;
}

namespace Plazza {

    class PizzaHandler {
        public:
            PizzaType parseType(const std::string &type_str);
            PizzaSize parseSize(const std::string &size_str);
            std::vector<std::shared_ptr<IPizza>> buildPizzas(const std::string &order);
            std::vector<std::shared_ptr<IPizza>> checkPizzaOrder(const std::string &order);
            std::string pizzaTypeToString(PizzaType type);
            std::string pizzaSizeToString(PizzaSize size);
        private:
    };

}
