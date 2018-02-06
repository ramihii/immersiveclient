CC=gcc
SRC=scan.c
OBJ=$(SRC:.c=.o)
CFLAGS=-Wall -I/home/okka/code/prefix/include
LDFLAGS=
LIBS=-lzbar -lpng

prog: $(OBJ)
	$(CC) -o prog $(OBJ) $(LDFLAGS) $(LIBS)

run: prog
	./prog test.png

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
