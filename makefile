all: | bin
	clang++ -arch arm64 -std=c++17 -I/opt/homebrew/Cellar/systemc/3.0.1/include/ -L/opt/homebrew/Cellar/systemc/3.0.1/lib/ -lsystemc -o bin/sim counter.cpp

bin:
	mkdir -p bin
