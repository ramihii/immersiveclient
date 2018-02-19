CC=gcc
SRC=scan.c
OBJ=$(SRC:.c=.o)
CFLAGS=-Wall -I/home/okka/code/prefix/include
LDFLAGS=/home/okka/code/prefix/lib/libzbar.so.0
LIBS=-lpng
RT=/home/okka/code/prefix/lib

prog: $(OBJ)
	$(CC) -o prog $(OBJ) $(LDFLAGS) $(LIBS)

run: prog
	LD_LIBRARY_PATH=$(RT) ./prog test.png

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm scan.o prog
