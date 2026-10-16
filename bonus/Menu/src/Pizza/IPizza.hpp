/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** IPizza
*/

#ifndef IPIZZA_HPP_
#define IPIZZA_HPP_

#include <string>
#include "Ingredients.hpp"

namespace Plazza {

     enum PizzaType
    {
        Regina = 1,
        Margarita = 2,
        Americana = 4,
        Fantasia = 8,
        AnneSaucisse = 16,
        PJSpecial = 32,
        Quentemmental = 64,
        Plantoine = 128,
        SimHotN = 256,
        ChloroFill = 512,
        WokNRollDamien = 1024,
        ValerianAvant = 2048,
        CyberJayce2077 = 4096
    };

    enum PizzaSize
    {
        S = 1,
        M = 2,
        L = 4,
        XL = 8,
        XXL = 16
    };
    

    class IPizza {
    public:
        virtual ~IPizza() = default;

        virtual PizzaType getType() = 0;
        virtual PizzaSize getSize() = 0;
        virtual int getOrderId() = 0;
        virtual bool isCooked() = 0;
        virtual int getCookingTime() = 0;
        virtual Plazza::ingredients getNeededIngredients() = 0;

        virtual void setOrderId(int orderId) = 0;
        virtual void setCooked(bool cooked) = 0;
        virtual void setCookingTime(int cookingTime) = 0;
        virtual void setNeededIngredients(const Plazza::ingredients &ingredients) = 0;
    };

}


#endif /* !IPIZZA_HPP_ */
