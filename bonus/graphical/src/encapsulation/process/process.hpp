/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** process
*/

#pragma once

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdexcept>

namespace Plazza {
    class Process {
        public:
            static pid_t fork();
            static pid_t waitpid(pid_t pid, int &status, int options);
            static void exit(int status);
            static int kill(pid_t pid, int signal);
            static pid_t getpid();
    };
}
