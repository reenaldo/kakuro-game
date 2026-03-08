# C++ compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Wconversion
LDFLAGS =

SRC = src/Kakuro.cpp src/GrilleLoaderJSON.cpp src/Solver.cpp
INC = -Iinclude -Ilib
OUT = kakuro

all:
	$(CXX) $(CXXFLAGS) $(SRC) $(INC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --errors-for-leak-kinds=all --error-exitcode=1 ./$(OUT)