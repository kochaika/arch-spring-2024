TARGET = app

CXX=g++
CXXFLAGS=-std=c++20 -Wall -pedantic -I./include

INC=-I./include


exec:
	$(CXX) $(CXXFLAGS) src/CPU.cpp src/main.cpp src/Bus.cpp -o emulator

all: test exec

test: clean test_SE
	./test_SE

test_SE:
	$(CXX) $(CXXFLAGS) tests/test_SE.cpp -o test_SE

clean:
	rm emulator test_SE 2> /dev/null ; true

