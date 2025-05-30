# 💥 Coup Game – Object-Oriented C++ Project

**Author:** Adham Hamoudy  
**Email:** adhamhamoudy3@gmail.com

## 🎯 Project Description

This project implements a simplified version of the *Coup* board game using **modern C++ (C++20)**. It focuses on **object-oriented programming**, **virtual inheritance**, and **the Rule of Three**, with additional support for a full **graphical user interface (GUI)** built using **SFML**.

Up to 6 players compete, using roles and strategic actions like tax, arrest, coup, or bribe. The last remaining active player wins the game.

---

## 🧩 Class and File Structure

| Module | Description |
|--------|-------------|
| `Player` | Base class with common player logic and actions |
| `Game` | Manages turn order, players, and game state |
| `Governor`, `Spy`, `Baron`, `General`, `Judge`, `Merchant` | Specialized roles with unique abilities |
| `main_gui.cpp` | GUI entry point (SFML-based) |
| `Demo.cpp` | Console demo |
| `test.cpp` | Unit tests using `doctest` |
| `makefile` | Compiles demo, tests, and supports valgrind/memory checks |

---

## 🧠 Object-Oriented Design Highlights

- ✅ **Virtual Inheritance** – All roles inherit virtually from `Player` to avoid ambiguity.
- ✅ **Rule of Three** – Copy constructor and assignment operator explicitly deleted in `Player`.
- ✅ **Polymorphism** – Role-specific behavior overridden via virtual functions.
- ✅ **Exception Safety** – All invalid game actions throw descriptive exceptions.

---

## 🖥️ GUI Features (via SFML)

- Turn-based flow: highlights current player
- Supports all actions: Gather, Tax, Bribe, Arrest, Sanction, Coup
- Special abilities:
  - Spy: reveals coin count & blocks arrest
  - Baron: invest 3 coins, gain 6
  - General: block coup by paying 5 coins
  - Governor: undo tax from another player
  - Judge: cancel bribe and penalize attacker
  - Merchant: bonus coin at turn start
- Eliminated players marked as **(Eliminated)** and shown in gray
- Logs all events and errors clearly
- Game Over detection with restart option

---

## 🧪 Testing and Validation

- Over **12 unit tests** using `doctest`
- Covers all actions, roles, exceptions, edge cases
- Validated with **valgrind** to ensure no memory leaks

---

## 🛠️ Build and Run Instructions

Make sure you have SFML installed (Linux: `sudo apt install libsfml-dev`).

```bash
# Build and run demo
make Main
./demo

# Run unit tests
make test

# Check memory safety
make valgrind

# Build and run GUI
make gui
./gui_exec

# Clean build files
make clean
