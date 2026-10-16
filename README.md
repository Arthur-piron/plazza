# The Plazza

![C++](https://img.shields.io/badge/C++17-00599C?logo=cplusplus&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?logo=linux&logoColor=black)
![SFML](https://img.shields.io/badge/SFML-bonus-8CC445)

A pizzeria simulation that exercises processes, threads and inter-process communication.
A reception shell takes orders; each **kitchen is a separate process**, each **cook is a thread**
of that kitchen's thread pool, and the reception talks to kitchens through **pipes**. Kitchens
are created on demand, share the load, restock their ingredients over time and close
themselves when idle.

Team project, Epitech 2nd year (May 2026).

```text
$ ./plazza 0.5 2 2000
Welcome to Stracotto Pizza! The ovens are hot and ready, what pizza(s) are you craving?

regina XXL x2; fantasia M x3; margarita S x1
You are order [#1], I'll call you when your pizza(s) are ready!

status
[KITCHEN #1]
2 pizza(s) pending
Cooks:
    Cook #1: Working...
    Cook #2: Working...
Ingredients:
    Dough:           1
    Tomato:          1
    ...
[KITCHEN #2]
1 pizza(s) pending
Cooks:
    Cook #1: Working...
    Cook #2: Sleeping...

[ORDER #1 COMPLETE] All pizzas are ready!
```

## Features

- **Order shell**: `TYPE SIZE xN` orders, several per line separated by `;`, plus `status`
  and `exit`.
- **One process per kitchen** (`fork`), created when existing kitchens are full
  (more than 2 × cooks pizzas waiting), closed after 5 seconds without work.
- **Load balancing**: each pizza goes to the kitchen with the lowest workload.
- **Thread pool per kitchen**: N cooks, synchronized with mutexes and condition variables;
  a separate thread restocks ingredients every N ms.
- **IPC**: pizzas and status snapshots are serialized and exchanged over pipes with
  `<<` / `>>` operators, with non-blocking reads.
- **Encapsulation** of every system primitive used (process, thread, mutex, condition
  variable, read/write, fcntl) in C++ classes.
- **Log file** (`log.txt`) of every order, kitchen and cooked pizza.
- **Bonuses**: a pixel-art graphical version with SFML (`bonus/graphical`) and an extended menu
  (`bonus/Menu`).

## My part

I set up the project structure and build, wrote the first reception shell and order parser,
the first kitchen thread model, then the **inter-process layer**: forking kitchens, the IPC
class and its serialization, non-blocking pipe reads, and the encapsulation of the system
calls it relies on.

## Stack

C++17 · POSIX processes, pipes and threads · Make · SFML (graphical bonus)

## Build and run

Prerequisites: a C++17 compiler and `make`, on Linux.

```bash
git clone https://github.com/Arthur-piron/plazza.git
cd plazza
make
./plazza <time_multiplier> <cooks_per_kitchen> <restock_time_ms>
# e.g. ./plazza 0.5 2 2000   (cooking twice as fast, 2 cooks per kitchen, restock every 2 s)
```

Available pizzas: `regina`, `margarita`, `americana`, `fantasia`. Sizes: `S`, `M`, `L`, `XL`, `XXL`.

Graphical version (needs `libsfml-dev`):

```bash
cd bonus/graphical && make && ./plazza 1 3 2000
```

See [bonus/graphical/doc/Readme.md](bonus/graphical/doc/Readme.md) and
[bonus/Menu/README.md](bonus/Menu/README.md) (adding a new pizza).
