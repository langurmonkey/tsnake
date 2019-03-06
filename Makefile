CPP=g++
CC=gcc
CFLAGS=-lncurses -Wall -g

snake: tsnake.cpp
	$(CPP) tsnake.cpp -o tsnake $(CFLAGS)
