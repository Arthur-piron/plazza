/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** Errors
*/

#ifndef ERRORS_HPP_
#define ERRORS_HPP_

#include <exception>
#include <string>

namespace Plazza {

class InvalidArgumentNumber : public std::exception
{
    public:
        InvalidArgumentNumber(const std::string &message) : errorMessage("Invalid argument number: " + message) {}

        const char *what() const noexcept override
        {
            return errorMessage.c_str();
        }

    private:
        std::string errorMessage;
};

class InvalidCooksPerKitchen : public std::exception
{
    public:
        InvalidCooksPerKitchen(const std::string &message) : errorMessage("Invalid cooks per kitchen: " + message) {}

        const char *what() const noexcept override
        {
            return errorMessage.c_str();
        }

    private:
        std::string errorMessage;
};

class InvalidTimeToRefill : public std::exception
{
    public:
        InvalidTimeToRefill(const std::string &message) : errorMessage("Invalid time to refill: " + message) {}

        const char *what() const noexcept override
        {
            return errorMessage.c_str();
        }

    private:
        std::string errorMessage;
};

class IncorrectType : public std::exception
{
    public:
        IncorrectType(const std::string &message) : errorMessage("Incorrect type: " + message) {}

        const char *what() const noexcept override
        {
            return errorMessage.c_str();
        }

    private:
        std::string errorMessage;
};

class IncorrectSize : public std::exception
{
    public:
        IncorrectSize(const std::string &message) : errorMessage("Incorrect size: " + message) {}

        const char *what() const noexcept override
        {
            return errorMessage.c_str();
        }

    private:
        std::string errorMessage;
};

class IncorrectQuantity : public std::exception
{
    public:
        IncorrectQuantity(const std::string &message) : errorMessage("Incorrect quantity: " + message) {}

        const char *what() const noexcept override
        {
            return errorMessage.c_str();
        }

    private:
        std::string errorMessage;
};

class AssetLoadError : public std::exception
{
    public:
        AssetLoadError(const std::string &path) : errorMessage("Failed to load asset: " + path) {}

        const char *what() const noexcept override
        {
            return errorMessage.c_str();
        }

    private:
        std::string errorMessage;
};

}

#endif /* !ERRORS_HPP_ */
