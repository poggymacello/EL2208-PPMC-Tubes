CC = gcc
CFLAGS = -g -Wall

all: main

files.o: files.c files.h
	$(CC) $(CFLAGS) -c files.c

lib.o: lib.c lib.h
	$(CC) $(CFLAGS) -c lib.c

main: main.c files.o lib.o
	$(CC) $(CFLAGS) -c main.c