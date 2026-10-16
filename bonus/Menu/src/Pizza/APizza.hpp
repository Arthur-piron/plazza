/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** APizza
*/

#ifndef APIZZA_HPP_
#define APIZZA_HPP_

#include "IPizza.hpp"

 namespace Plazza {

     class APizza : public IPizza {
        public:
            APizza(PizzaType type, PizzaSize size) : _type(type), _size(size) {}
            ~APizza() override = default;

            PizzaType getType() override { return _type; }
            PizzaSize getSize() override { return _size; }
            int getOrderId() override { return _orderId; }
            bool isCooked() override { return _cooked; }
            int getCookingTime() override { return _cookingTime; }
            Plazza::ingredients getNeededIngredients() override { return _neededIngredients; }

            void setOrderId(int newOrderId) override { _orderId = newOrderId; }
            void setCooked(bool newCooked) override { _cooked = newCooked; }
            void setCookingTime(int newCookingTime) override { _cookingTime = newCookingTime; }
            void setNeededIngredients(const Plazza::ingredients &ingredients) override { _neededIngredients = ingredients; }
            

        private:
            PizzaType _type;
            PizzaSize _size;
            int _orderId = -1;
            int _cookingTime = 0;
            bool _cooked = false;
            Plazza::ingredients _neededIngredients;
    };
}

#endif /* !APIZZA_HPP_ */
