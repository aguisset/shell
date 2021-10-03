CC=gcc
CFLAGS=-g -Wall

.PHONY: all
all: nyush

nyush: nyush.o parseCommand.o runCommand.o

nyush.o: nyush.c parseCommand.h runCommand.h

parseCommand.o: parseCommand.c parseCommand.h

runCommand.o: runCommand.c

.PHONY: clean
clean:
	rm -f *.o nyush
