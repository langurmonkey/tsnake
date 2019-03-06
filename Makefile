CPP=g++
CC=gcc
CFLAGS=-lncurses -Wall -g -std=c++17

tsnake: tsnake.cpp
	$(CPP) tsnake.cpp -o tsnake $(CFLAGS)
