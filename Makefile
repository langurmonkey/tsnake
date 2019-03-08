CPP=g++
CC=gcc
CFLAGS=-lncurses -Wall -g -std=c++17

default: tsnake clean

tsnake: tsnake.o InputParser.o
	$(CPP) tsnake.o InputParser.o -o tsnake $(CFLAGS)

tsnake.o: tsnake.cpp
	$(CPP) -c tsnake.cpp $(CFLAGS)

InputParser.o: InputParser.cpp
	$(CPP) -c InputParser.cpp

clean:
	rm *.o
