CC=gcc
CFLAGS=-g -Wall

all:
	$(CC) $(CFLAGS) src/utils/*/*.c src/json/*.c src/on.c src/main.c
