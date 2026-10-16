/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** main
*/

#include "Kitchen.hpp"
#include "Receptionist.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <csignal>
#include <fstream>
#include "Errors.hpp"
#include <sstream>

int main(int ac, char **av)
{
    srand(time(NULL));
    std::signal(SIGPIPE, SIG_IGN);
    try {
        Plazza::Receptionist receptionist;

        if (ac != 4) {
            std::cerr << "Usage: " << av[0] << " <time_multiplier> <cooks_per_kitchen> <time_to_refill>" << std::endl;
            throw Plazza::InvalidArgumentNumber("Expected 3 arguments, got " + std::to_string(ac - 1));
        }
        if (std::atof(av[1]) <= 0 || std::atoi(av[2]) <= 0 || std::atoi(av[3]) <= 0) {
            std::cerr << "Usage: " << av[0] << " <time_multiplier> <cooks_per_kitchen> <time_to_refill>" << std::endl;
            throw Plazza::InvalidArgumentNumber("All arguments must be positive values");
        }
        std::ofstream logFile("log.txt", std::ios::trunc);
        Plazza::Pizzeria pizzeria = {static_cast<float>(std::atof(av[1])), std::atoi(av[2]), std::atoi(av[3])};
        receptionist.handleOrder(pizzeria);
        return 0;
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 84;
    }
}
