/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** PizzaFactory
*/

#ifndef PIZZAFACTORY_HPP_
#define PIZZAFACTORY_HPP_

#include <memory>
#include "IPizza.hpp"

namespace Plazza {

class PizzaFactory {
public:
    PizzaFactory() = default;
    ~PizzaFactory() = default;
    std::shared_ptr<IPizza> createPizza(PizzaType type, PizzaSize size) const;

protected:
private:
};

}

#endif /* !PIZZAFACTORY_HPP_ */
