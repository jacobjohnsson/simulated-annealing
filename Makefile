CC=gcc
CFLAGS=-Wall
LFLAGS=-lm

make:
	$(CC) $(CFLAGS) -ggdb -O0 src/*{c,h} -o main.out $(LFLAGS)
