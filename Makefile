CC=gcc
CFLAGS=-g -Wall

.PHONY: all
all: nyush

nyush: nyush.o parseCommand.o runCommand.o

nyush.o: nyush.c runCommand.h parseCommand.h common.h

parseCommand.o: parseCommand.c parseCommand.h

runCommand.o: runCommand.c runCommand.h

.PHONY: clean
clean:
	rm -f *.o nyush
