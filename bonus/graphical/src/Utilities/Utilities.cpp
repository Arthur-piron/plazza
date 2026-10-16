/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** Utilities
*/


#include "Utilities.hpp"
#include <fstream>
#include <iostream>

void Plazza::Utilities::writeOutput(const std::string &output)
{
	std::ofstream file("log.txt", std::ios::app);

	if (!file.is_open()) {
		std::cerr << "Warning: Could not open log.txt for writing" << std::endl;
		return;
	}
	file << output << std::endl;
	file.close();
	
	std::cerr << output << std::endl;
}
