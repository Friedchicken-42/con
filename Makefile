CC := gcc
SRCDIR := src
INC :=-I include
CFLAGS=-g -Wall

all: json

.PHONY: con json dir clean

con: lib/libon.a build/main.o json
	mkdir -p bin
	$(CC) $(CFLAGS) $(INC) -o bin/con build/main.o lib/libon.a

build/main.o: src/main.c
	$(CC) $(CFLAGS) $(INC) -c -o build/main.o src/main.c

lib/libon.a: dir build/alloc.o build/string.o build/list.o build/hashmap.o build/on.o
	mkdir -p lib
	ar cr lib/libon.a build/alloc.o build/string.o build/list.o build/hashmap.o build/on.o

json: lib/libon.a build/json_load.o build/json_dump.o
	ar r lib/libon.a build/json_load.o build/json_dump.o

dir:
	mkdir -p build

build/alloc.o: src/alloc.c
	$(CC) $(CFLAGS) $(INC) -c -o build/alloc.o src/alloc.c

build/string.o: src/string.c
	$(CC) $(CFLAGS) $(INC) -c -o build/string.o src/string.c

build/list.o: src/list.c
	$(CC) $(CFLAGS) $(INC) -c -o build/list.o src/list.c

build/hashmap.o: src/hashmap.c
	$(CC) $(CFLAGS) $(INC) -c -o build/hashmap.o src/hashmap.c

build/on.o: src/on.c
	$(CC) $(CFLAGS) $(INC) -c -o build/on.o src/on.c

build/json_load.o: src/json/json_load.c
	$(CC) $(CFLAGS) $(INC) -c -o build/json_load.o src/json/json_load.c

build/json_dump.o: src/json/json_dump.c
	$(CC) $(CFLAGS) $(INC) -c -o build/json_dump.o src/json/json_dump.c

clean:
	rm -rf bin lib build
