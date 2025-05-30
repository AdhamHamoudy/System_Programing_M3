# Email: adhamhamoudy3@gmail.com

CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic
CXXFLAGS_GUI = -std=c++20 -Wall -Wextra -pedantic
INCLUDES = -Iinclude
SRC_DIR = src
TEST_DIR = test
GUI_SRC = main_gui.cpp
MAIN_SRC = main.cpp
DEMO_SRC = Demo.cpp

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
HEADERS = $(wildcard include/*.hpp)

TEST_EXE = test_exec
DEMO_EXE = demo
MAIN_EXE = main_exec
GUI_EXE = gui_exec

SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system

.PHONY: test demo main valgrind clean gui

# === Build and run main.cpp ===
main:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(MAIN_SRC) $(SOURCES) -o $(MAIN_EXE)
	./$(MAIN_EXE)

# === Build and run demo ===
demo:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEMO_SRC) $(SOURCES) -o $(DEMO_EXE)
	./$(DEMO_EXE)

# === Build and run tests ===
test:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(TEST_DIR)/test.cpp $(SOURCES) -o $(TEST_EXE)
	./$(TEST_EXE)

# === Build GUI ===
gui:
	$(CXX) $(CXXFLAGS_GUI) $(INCLUDES) $(GUI_SRC) $(SOURCES) -o $(GUI_EXE) $(SFML_FLAGS)

# === Run valgrind ===
valgrind: test
	valgrind --leak-check=full --track-origins=yes ./$(TEST_EXE)

# === Clean all builds ===
clean:
	rm -f $(TEST_EXE) $(DEMO_EXE) $(MAIN_EXE) $(GUI_EXE) *.o core
