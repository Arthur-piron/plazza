/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** Pizzas
*/

#ifndef PIZZAS_HPP_
#define PIZZAS_HPP_

#include "APizza.hpp"
#include "IPizza.hpp"
#include "Kitchen.hpp"

namespace Plazza {

    class MargaritaPizza : public APizza {
        public:
            MargaritaPizza(PizzaSize size) : APizza(PizzaType::Margarita, size) {
                setCookingTime(1000);
                setNeededIngredients(Plazza::ingredients{
                    1, // dough
                    1, // tomato
                    1, // gruyere
                    0, // sausage
                    0, // ham
                    0, // mushroom
                    0, // steak
                    0, // eggplant
                    0, // goatCheese
                    0, // chiefLove
                    0, // chili
                    0, // tofu
                    0  // olives
                });
            }
            ~MargaritaPizza() override = default;
    };

    class ReginaPizza : public APizza {
        public:
            ReginaPizza(PizzaSize size) : APizza(PizzaType::Regina, size) {
                setCookingTime(2000);
                setNeededIngredients(Plazza::ingredients{
                    1, // dough
                    1, // tomato
                    1, // gruyere
                    0, // sausage
                    1, // ham
                    1, // mushroom
                    0, // steak
                    0, // eggplant
                    0, // goatCheese
                    0, // chiefLove
                    0, // chili
                    0, // tofu
                    0  // olives
                });
            }
            ~ReginaPizza() override = default;
    };

    class AmericanaPizza : public APizza {
        public:
            AmericanaPizza(PizzaSize size) : APizza(PizzaType::Americana, size) {
                setCookingTime(2000);
                setNeededIngredients(Plazza::ingredients{
                    1, // dough
                    1, // tomato
                    1, // gruyere
                    0, // sausage
                    0, // ham
                    0, // mushroom
                    1, // steak
                    0, // eggplant
                    0, // goatCheese
                    0, // chiefLove
                    0, // chili
                    0, // tofu
                    0  // olives
                });
            }
            ~AmericanaPizza() override = default;
    };

    class FantasiaPizza : public APizza {
        public:
            FantasiaPizza(PizzaSize size) : APizza(PizzaType::Fantasia, size) {
                setCookingTime(4000);
                setNeededIngredients(Plazza::ingredients{
                    1, // dough
                    1, // tomato
                    0, // gruyere
                    0, // sausage
                    0, // ham
                    0, // mushroom
                    0, // steak
                    1, // eggplant
                    1, // goatCheese
                    1, // chiefLove
                    0, // chili
                    0, // tofu
                    0  // olives
                });
            }
            ~FantasiaPizza() override = default;
    };

    class AnneSaucissePizza : public APizza {
        public:
            AnneSaucissePizza(PizzaSize size) : APizza(PizzaType::AnneSaucisse, size) {
                setCookingTime(2000);
                setNeededIngredients(Plazza::ingredients{
                    1, // dough
                    1, // tomato
                    1, // gruyere
                    1, // sausage
                    0, // ham
                    0, // mushroom
                    0, // steak
                    0, // eggplant
                    0, // goatCheese
                    0, // chiefLove
                    0, // chili
                    0, // tofu
                    0  // olives
                });
            }
            ~AnneSaucissePizza() override = default;
    };

    // NEW PIZZAS
    class PJSpecialPizza : public APizza {
        public:
            PJSpecialPizza(PizzaSize size) : APizza(PizzaType::PJSpecial, size) {
                setCookingTime(3000);
                setNeededIngredients(Plazza::ingredients{
                    1,
                    1,
                    1,
                    3,
                    3,
                    1,
                    2,
                    0,
                    0,
                    2,
                    1,
                    0,
                    0
                });
            }
            ~PJSpecialPizza() override = default;
    };

    class QuentemmentalPizza : public APizza {
        public:
            QuentemmentalPizza(PizzaSize size) : APizza(PizzaType::Quentemmental, size) {
                setCookingTime(1500);
                setNeededIngredients(Plazza::ingredients{
                    1,
                    1,
                    5,
                    0,
                    0,
                    1,
                    0,
                    0,
                    4,
                    3,
                    0,
                    0,
                    1
                });
            }
            ~QuentemmentalPizza() override = default;
    };

    class PlantoinePizza : public APizza {
        public:
            PlantoinePizza(PizzaSize size) : APizza(PizzaType::Plantoine, size) {
                setCookingTime(2200);
                setNeededIngredients(Plazza::ingredients{
                    1,
                    2,
                    2,
                    0,
                    0,
                    3,
                    0,
                    4,
                    2,
                    2,
                    0,
                    2,
                    3
                });
            }
            ~PlantoinePizza() override = default;
    };

    class SimHotNPizza : public APizza {
        public:
            SimHotNPizza(PizzaSize size) : APizza(PizzaType::SimHotN, size) {
                setCookingTime(1800);
                setNeededIngredients(Plazza::ingredients{
                    1,
                    2,
                    2,
                    2,
                    1,
                    1,
                    1,
                    0,
                    0,
                    2,
                    5,
                    0,
                    1
                });
            }
            ~SimHotNPizza() override = default;
    };

    class ChloroFillPizza : public APizza {
        public:
            ChloroFillPizza(PizzaSize size) : APizza(PizzaType::ChloroFill, size) {
                setCookingTime(2500);
                setNeededIngredients(Plazza::ingredients{
                    1,
                    1,
                    1,
                    0,
                    0,
                    4,
                    0,
                    3,
                    2,
                    1,
                    4,
                    0,
                    5
                });
            }
            ~ChloroFillPizza() override = default;
    };

    class WokNRollDamienPizza : public APizza {
        public:
            WokNRollDamienPizza(PizzaSize size) : APizza(PizzaType::WokNRollDamien, size) {
                setCookingTime(2000);
                setNeededIngredients(Plazza::ingredients{
                    1,
                    1,
                    2,
                    0,
                    0,
                    2,
                    1,
                    1,
                    0,
                    2,
                    2,
                    4,
                    1
                });
            }
            ~WokNRollDamienPizza() override = default;
    };

    class ValerianAvantPizza : public APizza {
        public:
            ValerianAvantPizza(PizzaSize size) : APizza(PizzaType::ValerianAvant, size) {
                setCookingTime(2100);
                setNeededIngredients(Plazza::ingredients{
                    1,
                    2,
                    1,
                    1,
                    1,
                    3,
                    2,
                    1,
                    0,
                    5,
                    1,
                    5,
                    0
                });
            }
            ~ValerianAvantPizza() override = default;
    };

    class CyberJayce2077Pizza : public APizza {
        public:
            CyberJayce2077Pizza(PizzaSize size) : APizza(PizzaType::CyberJayce2077, size) {
                setCookingTime(3500);
                setNeededIngredients(Plazza::ingredients{
                    1,
                    1,
                    3,
                    1,
                    1,
                    2,
                    1,
                    1,
                    3,
                    10,
                    3,
                    3,
                    3
                });
            }
            ~CyberJayce2077Pizza() override = default;
    };

}

#endif /* !PIZZAS_HPP_ */
