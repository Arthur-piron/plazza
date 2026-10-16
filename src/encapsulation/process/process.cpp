/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** process
*/

#include "process.hpp"
#include <cerrno>

namespace Plazza {
    pid_t Process::fork()
    {
        pid_t pid = ::fork();
        if (pid == -1) {
            throw std::runtime_error("Failed to fork process");
        }
        return pid;
    }

    pid_t Process::waitpid(pid_t pid, int &status, int options)
    {
        pid_t result = ::waitpid(pid, &status, options);
        if (result == -1 && errno != ECHILD) {
            throw std::runtime_error("Failed to wait for process");
        }
        return result;
    }

    void Process::exit(int status)
    {
        ::exit(status);
    }

    int Process::kill(pid_t pid, int signal)
    {
        int result = ::kill(pid, signal);
        if (result == -1) {
            throw std::runtime_error("Failed to kill process");
        }
        return result;
    }

    pid_t Process::getpid()
    {
        return ::getpid();
    }
}
