all: ackley

ackley: ackley.cpp
	g++ -std=c++14 -pthread -O3 ackley.cpp -o ackley -lboost_program_options

clean:
	rm -rf ackley