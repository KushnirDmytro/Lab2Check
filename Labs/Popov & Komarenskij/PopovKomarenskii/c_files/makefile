makefile:
all: target1 target2

target1: threads.o
	g++ -g -o threads.out threads.o

target2: integrate.o
	g++ -g -o integrate.out integrate.o 

threads.o: threads.cpp
	g++ -g -c threads.cpp -O3 -Og -std=c++11 -pthread

integrate.o: integrate.cpp
	g++ -g -c integrate.cpp -O3 -Og -std=c++11 