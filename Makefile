CPP=g++
CC=gcc
CFLAGS=-lncurses -Wall -g -std=c++17
INSTALL_PATH=/usr/local

default: tsnake 

tsnake: tsnake.o InputParser.o
	$(CPP) tsnake.o InputParser.o -o tsnake $(CFLAGS)

tsnake.o: tsnake.cpp
	$(CPP) -c tsnake.cpp $(CFLAGS)

InputParser.o: InputParser.cpp
	$(CPP) -c InputParser.cpp

man: tsnake.1

tsnake.1: tsnake
	help2man --include tsnake.h2m -o tsnake.1 ./tsnake

.PHONY: install
install: tsnake tsnake.1
	mkdir -p $(INSTALL_PATH)/man/man1/
	cp tsnake.1 $(INSTALL_PATH)/man/man1/
	mkdir -p $(INSTALL_PATH)/bin/
	cp tsnake $(INSTALL_PATH)/bin/

.PHONY: uninstall
uninstall:
	rm $(INSTALL_PATH)/man/man1/tsnake.1
	rm $(INSTALL_PATH)/bin/tsnake

.PHONY: clean
clean:
	rm -f tsnake *.o
