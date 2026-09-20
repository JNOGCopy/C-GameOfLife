CC = gcc
CFLAGS = -Wall -g

build: src/main.c
	gcc $(CFLAGS) -Iinclude -lSDL3 -lSDL3_ttf src/main.c -o build/out/lifegame.elf
run: build
	./build/out/lifegame.elf
