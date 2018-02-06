CC=gcc
SRC=create.c
OBJ=$(SRC:.c=.o)
LIBS=-lqrencode -lpng

prog: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) $(OBJ) -o prog

run: prog
	./prog

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
