/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** Pizza
*/

#include "Kitchen.hpp"
#include "PizzaFactory.hpp"
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <unordered_map>
#include <memory>
#include <climits>

namespace Plazza {

PizzaType PizzaHandler::parseType(const std::string &name)
{
    static const std::unordered_map<std::string, PizzaType> types = {
        {"regina", Regina},
        {"margarita", Margarita},
        {"americana", Americana},
        {"fantasia", Fantasia},
    };
    std::string normalizedName = name;

    for (char &character : normalizedName)
        character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));

    auto iterator = types.find(normalizedName);

    if (iterator == types.end())
        return static_cast<PizzaType>(0);
    return iterator->second;
}

PizzaSize PizzaHandler::parseSize(const std::string &size)
{
    static const std::unordered_map<std::string, PizzaSize> sizes = {
        {"S", S}, {"M", M}, {"L", L}, {"XL", XL}, {"XXL", XXL},
    };
    auto iterator = sizes.find(size);

    if (iterator == sizes.end())
        return static_cast<PizzaSize>(0);
    return iterator->second;
}

std::vector<std::shared_ptr<IPizza>> PizzaHandler::buildPizzas(const std::string &order)
{
    std::istringstream iss(order);
    std::string typeStr;
    std::string sizeStr;
    std::string quantityStr;
    std::vector<std::shared_ptr<IPizza>> pizzas;

    if (!(iss >> typeStr >> sizeStr >> quantityStr)) {
        std::cout << "\nI can't take that order. Please enter an order in the format: <type> <size> <quantity>" << std::endl << std::endl;
        return {};
    }

    std::string extraToken;
    if (iss >> extraToken) {
        std::cout << "\nI can't take that order. Please enter exactly: <type> <size> x<quantity>" << std::endl << std::endl;
        return {};
    }

    if (quantityStr[0] != 'x') {
        std::cout << "\nI can't take that order. The quantity must start with 'x'. Please enter an order in the format: <type> <size> x<quantity>" << std::endl;
        return {};
    }

    const std::string quantityDigits = quantityStr.substr(1);

    if (quantityDigits.empty() || quantityDigits.find_first_not_of("0123456789") != std::string::npos || quantityDigits[0] == '0') {
        std::cout << "\nI can't take that order. The quantity must be a positive integer written as x<quantity>." << std::endl;
        return {};
    }

    int quantity = 0;

    try {
        quantity = std::stoi(quantityDigits);
    } catch (const std::exception &) {
        std::cout << "\nI can't take that order. The quantity is too large or invalid." << std::endl;
        return {};
    }

    if (quantity <= 0) {
        std::cout << "\n can't take that order. The quantity must be a positive number. Please enter an order in the format: <type> <size> x<quantity>" << std::endl;
        return {};
    }

    PizzaType type = parseType(typeStr);
    if (type == 0) {
        std::cout << "\nI can't take that order. I don't recognize that pizza type. Please enter a valid pizza type (Regina, Margarita, Americana, Fantasia)." << std::endl;
        return {};
    }

    PizzaSize size = parseSize(sizeStr);
    if (size == 0) {
        std::cout << "\nI can't take that order. I don't recognize that pizza size. Please enter a valid pizza size (S, M, L, XL, XXL)." << std::endl;
        return {};
    }
    
    PizzaFactory factory;
    
    for (int i = 0; i < quantity; i++)
        pizzas.push_back(factory.createPizza(type, size));

    return pizzas;
}

std::vector<std::shared_ptr<IPizza>> PizzaHandler::checkPizzaOrder(const std::string &order)
{
    std::istringstream iss(order);
    std::string newPizza;
    std::vector<std::shared_ptr<IPizza>> newPizzas;

    while (std::getline(iss, newPizza, ';')) {
        std::vector<std::shared_ptr<IPizza>> pizzas = buildPizzas(newPizza);

        if (pizzas.empty())
            return {};

        for (auto &pizza : pizzas)
            newPizzas.push_back(pizza);
    }

    if (newPizzas.empty())
        return {};

    return newPizzas;
}

std::string PizzaHandler::pizzaTypeToString(Plazza::PizzaType type) {
    switch (type) {
        case Plazza::Regina:
            return "Regina";
        case Plazza::Margarita:
            return "Margarita";
        case Plazza::Americana:
            return "Americana";
        case Plazza::Fantasia:
            return "Fantasia";
        default:
            return "Unknown";
    }
}

std::string PizzaHandler::pizzaSizeToString(Plazza::PizzaSize size) {
    switch (size) {
        case Plazza::S:
            return "S";
        case Plazza::M: 
            return "M";
        case Plazza::L: 
            return "L";
        case Plazza::XL: 
            return "XL";
        case Plazza::XXL: 
            return "XXL";
        default: 
            return "Unknown";
    }
}

}
