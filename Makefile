##
## EPITECH PROJECT, 2026
## PIZZA_DE_LA_MAMA
## File description:
## Makefile
##

NAME = plazza

CXX = clang++

SRC =	src/main.cpp \
		src/Kitchen/Kitchen.cpp \
		src/Kitchen/Cook.cpp \
		src/Pizza/PizzaHandler.cpp \
		src/Receptionist/Receptionist.cpp \
		src/ThreadPool/ThreadPool.cpp \
		src/PizzaFactory/PizzaFactory.cpp \
		src/encapsulation/read/read.cpp \
		src/encapsulation/write/write.cpp \
		src/encapsulation/fcntl/fcntl.cpp \
		src/encapsulation/condition_variable/condition_variable.cpp \
		src/encapsulation/mutex/mutex.cpp \
		src/encapsulation/process/process.cpp \
		src/encapsulation/thread/thread.cpp \
		src/IPC/IPC.cpp \
		src/Utilities/Utilities.cpp \

INCLUDES =	-Isrc \
			-Isrc/Kitchen \
			-Isrc/Pizza \
			-Isrc/Errors \
			-Isrc/Receptionist \
			-Isrc/ThreadPool \
			-Isrc/PizzaFactory \
			-Isrc/encapsulation \
			-Isrc/encapsulation/read \
			-Isrc/encapsulation/write \
			-Isrc/encapsulation/fcntl \
			-Isrc/encapsulation/condition_variable \
			-Isrc/encapsulation/mutex \
			-Isrc/encapsulation/process \
			-Isrc/encapsulation/thread \
			-Isrc/IPC \
			-Isrc/Utilities \

OBJ = $(SRC:.cpp=.o)

CXXFLAGS = -Wall -Wextra -std=c++17 -pthread $(INCLUDES)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(OBJ) -o $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

debug: re

.PHONY: all clean fclean re debug