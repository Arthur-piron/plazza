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
                    1,
                    1,
                    1,
                    0,
                    0,
                    0,
                    0,
                    0
                });
            }
            ~MargaritaPizza() override = default;
    };

    class ReginaPizza : public APizza {
        public:
            ReginaPizza(PizzaSize size) : APizza(PizzaType::Regina, size) {
                setCookingTime(2000);
                setNeededIngredients(Plazza::ingredients{
                    1,
                    1,
                    1,
                    1,
                    1,
                    0,
                    0,
                    0
                });
            }
            ~ReginaPizza() override = default;
    };


    class AmericanaPizza : public APizza {
        public:
            AmericanaPizza(PizzaSize size) : APizza(PizzaType::Americana, size) {
                setCookingTime(2000);
                setNeededIngredients(Plazza::ingredients{
                    1,
                    1,
                    1,
                    0,
                    0,
                    1,
                    0,
                    0
                });
            }
            ~AmericanaPizza() override = default;
    };

    class FantasiaPizza : public APizza {
        public:
            FantasiaPizza(PizzaSize size) : APizza(PizzaType::Fantasia, size) {
                setCookingTime(4000);
                setNeededIngredients(Plazza::ingredients{
                    1,
                    1,
                    0,
                    0,
                    0,
                    0,
                    1,
                    1
                });
            }
            ~FantasiaPizza() override = default;
    };

}

#endif /* !PIZZAS_HPP_ */
