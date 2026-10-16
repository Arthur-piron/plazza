/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** PizzaFactory
*/

#include "PizzaFactory.hpp"
#include "Pizzas.hpp"

namespace Plazza {

std::shared_ptr<IPizza> PizzaFactory::createPizza(PizzaType type, PizzaSize size) const
{
	switch (type) {
		case Regina:
			return std::make_shared<ReginaPizza>(size);
		case Margarita:
			return std::make_shared<MargaritaPizza>(size);
		case Americana:
			return std::make_shared<AmericanaPizza>(size);
		case Fantasia:
			return std::make_shared<FantasiaPizza>(size);
		default:
			return nullptr;
	}
}

}

