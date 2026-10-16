# 🍕 Plazza - Graphical Pizza Shop Simulator

An interactive graphical simulation of a pizzeria with dynamic kitchens, cooks, and order management. This is the **bonus graphical version** of the Plazza project, featuring real-time visualization of your pizza shop operations.

---

## 📋 What is Plazza?

Plazza is a **multi-threaded, multi-process pizzeria simulator** that teaches you about:
- **Process management** - Kitchens are spawned as child processes
- **Thread synchronization** - Cooks work as threads with proper synchronization
- **Inter-process communication (IPC)** - Kitchens and the reception communicate seamlessly
- **Load balancing** - Orders are distributed fairly across kitchens
- **Real-time graphics** - Visual feedback through a graphical interface

Think of it as running a real pizza restaurant from your terminal! Orders come in, kitchens cook pizzas (with cooks working in parallel), and you can watch the whole operation happen in real-time with fun pixel art animations.

---

## 🎮 Getting Started

### Prerequisites

You'll need the SFML installed:

```bash
# On Ubuntu/Debian
sudo apt-get install libsfml-dev

# On Fedora
sudo dnf install SFML-devel
```

### Compilation

The project uses a standard Makefile with the following rules:

```bash
# Build the project
make

# Clean object files
make clean

# Remove everything (objects + binary)
make fclean

# Clean and rebuild
make re

# Build in debug mode
make debug
```

### Running Plazza

The graphical interface requires three command-line arguments:

```bash
./plazza <time_multiplier> <cooks_per_kitchen> <refill_time_ms>
```

**Parameters explained:**

- **`<time_multiplier>`** (float): Multiplier for cooking times
  - Use `1` for normal speed (as specified in recipes)
  - Use `0.5` for twice as fast
  - Use `2` for half as fast
  - Must be > 0

- **`<cooks_per_kitchen>`** (int): Number of cooks working in each kitchen
  - Example: `2` means each kitchen starts with 2 cooks
  - More cooks = faster pizza production, but they'll need ingredients more often

- **`<refill_time_ms>`** (int): Milliseconds between ingredient refills
  - Example: `2000` means ingredients refill every 2 seconds
  - Affects how often cooks get fresh stock

**Example:**
```bash
./plazza 1 2 2000
# Creates a pizzeria where:
# - Pizzas cook at normal speed
# - Each kitchen has 2 cooks
# - Ingredients refill every 2 seconds
```

---

## 🍕 How The Pizzeria Works

### Architecture

```
┌─────────────────────────────────┐
│   Reception (Main Process)       │
│ - Accepts pizza orders from user │
│ - Distributes to kitchens        │
│ - Shows order status             │
└──────────┬──────────────────────┘
           │ (IPC)
    ┌──────┼──────┐
    │      │      │
┌───▼─--─┐ ┌─▼─----─┐ ┌─▼─-----─┐
│Kitchen1│ │Kitchen2│ │Kitchen3 │
│ (Child │ │ (Child │ │ (Child  │
│Process)│ │Process)│ │Process) │
└───┬─--─┘ └─-──----┘ └─-──-----┘
    |
    │  (Threads)
  ┌─┴─---┬─-----┬─-─┐
  │      │      │   │
 [Cook][Cook][Cook][Cook]...
```

### The Reception
- The heart of your pizzeria
- Listens to your orders and distributes them fairly
- Creates new kitchens when existing ones get too busy
- Keeps track of all orders and displays when they're ready

### Kitchens
- **Child processes** that spawn when needed
- Each has a **ThreadPool** managing multiple **Cook threads**
- Possess an **ingredient stock** that regenerates over time
- Can't accept more than `2 × number_of_cooks` pizzas at once
- Automatically close if none of the cooks are working during 5+ seconds

### Cooks
- **Threads** that prepare pizzas one at a time
- Display fun and hand-made animations while working
- Refuse to work if ingredients run out (wait for refill)
- Special animation when finishing a "Fantasia" pizza (to show ingredient "Chief Love")

### Pizzas Supported

| Pizza | Ingredients | Cook Time |
|-------|-------------|-----------|
| **Margarita** | Dough, Tomato, Gruyere | 1 sec × multiplier |
| **Regina** | Dough, Tomato, Gruyere, Ham, Mushrooms | 2 sec × multiplier |
| **Americana** | Dough, Tomato, Gruyere, Steak | 2 sec × multiplier |
| **Fantasia** | Dough, Tomato, Eggplant, Goat Cheese, Chief Love | 4 sec × multiplier |

---

## 🎨 Graphical Interface

### Main Screen

The graphical window shows:
- **Kitchen view** with animated cooks
- **Order queue** showing customer orders
- **Ingredient status** for the current kitchen
- **Input bar** to type orders
- **Exit button** to quit cleanly
- **Sound button** to turn on/off sound effects and music

### Kitchen Navigation

- **Arrow Keys** (← →) to switch between kitchens
- **Kitchen Counter** shows which kitchen you're viewing (Kitchen 1/5, etc.)
- See all cooks working in each kitchen in real-time

### Placing Orders

1. **Click in the input bar** at the bottom of the screen
2. **Type your order** using the grammar below
3. **Press Enter** to submit your order

### Order Grammar

Orders follow this pattern:

```
<TYPE> <SIZE> x<QUANTITY> [; <TYPE> <SIZE> x<QUANTITY>]*
```

**Example orders:**
```
regina XL x2
margarita S x1; fantasia M x3
americana L x5; regina XXL x2; margarita M x1
```

**Valid values:**
- **TYPE**: `margarita`, `regina`, `americana`, `fantasia` (case-insensitive)
- **SIZE**: `S`, `M`, `L`, `XL`, `XXL`
- **QUANTITY**: `x1` to `x99` (e.g., `x7` for 7 pizzas)

Invalid orders will be rejected with a visible error message.

### Commands

You can type two special commands in the input bar:

#### `status`
```
status
```
Displays a detailed report in the terminal:
- Number of active kitchens
- Each kitchen's occupancy
- Ingredient stock levels for each kitchen
- Total pizzas ready and still cooking

#### `exit`
```
exit
```
Gracefully closes your pizzeria:
- Tells all kitchens to finish current pizzas
- Displays final order summary
- Saves everything to `log.txt`

### Cook Animation States

Cooks display different animations based on what they're doing:

| State | Animation | Meaning |
|-------|-----------|---------|
| 🙂 **Idle** | Cook standing calmly | Waiting for a pizza to cook |
| 👨‍🍳 **Cooking** | Cook working at counter | Currently preparing a pizza |
| 😴 **Sleeping** | Cook sleeping | Waiting for ingredients to refill |
| 👨‍🍳💋 **Chef's Kiss** | Cook celebrating | Pizza necessiting chief love finished |

Each animation loops smoothly - you'll see the cook's expression change as they work through orders !

### Order Tracking

- **Orders appear** as customer icons in the queue
- **Customers leave** satisfied with their pizzas!
- **Order tracking** written in the terminal and log file

---

## 📊 Behind The Scenes

### Process & Thread Management

- **Reception**: Main process that manages everything
- **Kitchens**: Child processes created on demand via `fork()`
- **Cooks**: Threads within each kitchen, scheduled by a ThreadPool
- **Synchronization**: Uses mutexes and condition variables for thread-safe operations
- **IPC**: Named pipes (FIFOs) for communication between processes

### Load Balancing

The system intelligently distributes orders:
- Pizzas from the same order go to different kitchens when possible
- Kitchens are chosen to balance their workload
- New kitchens spawn only when necessary
- Kitchens close automatically when not needed

### Ingredient System

Each kitchen starts with **5 units** of each ingredient:
- Dough, Tomato, Gruyere, Ham, Mushrooms, Steak, Eggplant, Goat Cheese

Ingredients refill periodically (based on `refill_time_ms` parameter):
- +1 unit of each ingredient every N milliseconds
- Maximum capacity: varies per ingredient
- Cooks wait if ingredients run out (enter sleeping state)

---

## 📝 Logging

All order information is logged to `log.txt`:
- Order placement timestamp
- Pizza details (type, size, quantity)
- Cooking start/end times
- Final status

Check this file to review your pizzeria's operations !

---

## 📚 Technical Stack

- **Language**: C++17
- **Graphics**: SFML (Simple and Fast Multimedia Library)
- **Concurrency**: POSIX threads + STL threads
- **IPC**: Named pipes (FIFO)
- **Build**: Makefile with clang++

---

## 🎓 Educational Goals

This graphical bonus teaches:
- ✅ Real-world load balancing
- ✅ Graphics programming with SFML
- ✅ C++ OOP and design patterns
- ✅ Real-life comportemental simulation
- ✅ Pixel art design and animation

---

## 👨‍💼 Credits

**Project**: Plazza - Graphical Pizza Shop Simulator  
**School**: EPITECH Lille  
**Module**: G-CCP-400 (C++ Concurrency Programming)  
**Academic Year**: 2025-2026, Semester 4  
**Developer**: Arthur Vignes

This project is part of the EPITECH curriculum and serves as a comprehensive introduction to multi-threaded and multi-process programming in C++. It combines theoretical concepts with practical implementation and visual feedback through a custom graphical interface.

---

## 🎨 Visual Assets

**All sprites, animations, and graphical assets in this project have been hand-crafted and created from scratch**, including:

- 🧑‍🍳 **Cook sprites** - Multiple animation states (idle, cooking, sleeping, chef's kiss)
- 👥 **Customer characters** - Order queue visualization
- 🏠 **Kitchen background** - Cozy pizzeria setting
- 📦 **Ingredient panel** - Visual ingredient inventory display
- 🎯 **UI elements** - Buttons, input bar, kitchen navigator
- 🎬 **Animations** - Smooth frame-by-frame transitions for all characters

While some visual references were used as design inspiration, every pixel was personally crafted and animated to bring this virtual pizzeria to life !

---

**Enjoy running your virtual pizzeria!** 🍕✨
