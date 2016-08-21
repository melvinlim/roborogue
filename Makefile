CC=gcc
CFLAGS=-O0 -I. -g
#DEPS = heap.h stack.h encode.h
#OBJ = heap.o stack.o encode.o main.o
DEPS = sight.h decode.h
OBJ = sight.o decode.o main.o
TARGS = main
NAME = robo

all: $(TARGS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $(NAME) $^ $(CFLAGS)
	#$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o *~ a.out $(NAME) 
