# Email: adhamhamoudy3@gmail.com

CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic
CXXFLAGS_GUI = -std=c++20 -Wall -Wextra -pedantic
INCLUDES = -Iinclude
SRC_DIR = src
TEST_DIR = test
GUI_SRC = main_gui.cpp

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
HEADERS = $(wildcard include/*.hpp)

TEST_SRC = $(TEST_DIR)/test.cpp
DEMO_SRC = Demo.cpp
TEST_EXE = test_exec
DEMO_EXE = demo
MAIN_EXE = main
GUI_EXE = gui_exec

SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system

.PHONY: test demo valgrind clean gui

# === Build and run tests ===
test:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(TEST_SRC) $(SOURCES) -o $(TEST_EXE)
	./$(TEST_EXE)

# === Build and run demo ===
demo:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEMO_SRC) $(SOURCES) -o $(DEMO_EXE)
	./$(DEMO_EXE)

# === Build and run GUI ===
gui: $(GUI_SRC) $(SOURCES)
	$(CXX) $(CXXFLAGS_GUI) $(INCLUDES) $(GUI_SRC) $(SOURCES) -o $(GUI_EXE) $(SFML_FLAGS)

# === Run valgrind on tests ===
valgrind: test
	valgrind --leak-check=full --track-origins=yes ./$(TEST_EXE)

# === Clean all build artifacts ===
clean:
	rm -f $(TEST_EXE) $(DEMO_EXE) $(GUI_EXE) *.o core
