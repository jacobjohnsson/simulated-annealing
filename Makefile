CC=gcc
CFLAGS=-Wall
LFLAGS=-lm

make:
	$(CC) $(CFLAGS) -ggdb -O0 src/main.c src/queens.c src/queens.h -o main.out $(LFLAGS)
