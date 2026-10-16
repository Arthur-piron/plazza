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
		case AnneSaucisse:
			return std::make_shared<AnneSaucissePizza>(size);
		case PJSpecial:
			return std::make_shared<PJSpecialPizza>(size);
		case Quentemmental:
			return std::make_shared<QuentemmentalPizza>(size);
		case Plantoine:
			return std::make_shared<PlantoinePizza>(size);
		case SimHotN:
			return std::make_shared<SimHotNPizza>(size);
		case ChloroFill:
			return std::make_shared<ChloroFillPizza>(size);
		case WokNRollDamien:
			return std::make_shared<WokNRollDamienPizza>(size);
		case ValerianAvant:
			return std::make_shared<ValerianAvantPizza>(size);
		case CyberJayce2077:
			return std::make_shared<CyberJayce2077Pizza>(size);
		default:
			return nullptr;
	}
}

}

