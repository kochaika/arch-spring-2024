TARGET = app

CXX=g++
CXXFLAGS=-std=c++20 -Wall -pedantic -I./include

INC=-I./include


exec:
	$(CXX) $(CXXFLAGS) src/CPU.cpp src/main.cpp src/Bus.cpp -o exec

all: test exec

test: clean test_SE
	./test_SE

test_SE:
	$(CXX) $(CXXFLAGS) tests/test_SE.cpp -o test_SE

clean:
	rm exec test_SE || true

