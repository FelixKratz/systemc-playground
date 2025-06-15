all: *.hpp *.cpp | bin
	clang++ -arch arm64 -std=c++17 -I/opt/homebrew/Cellar/systemc/3.0.1/include/ -L/opt/homebrew/Cellar/systemc/3.0.1/lib/ -lsystemc -o bin/sim simulation.cpp

test: *.hpp *.cpp | bin
	clang++ -arch arm64 -std=c++17 -I/opt/homebrew/Cellar/systemc/3.0.1/include/ -L/opt/homebrew/Cellar/systemc/3.0.1/lib/ -lsystemc -o bin/sim test.cpp

bin:
	mkdir -p bin
