CC=gcc
CFLAGS=-O4 -I. -g
DEPS = move.h list.h graph.h sight.h decode.h
OBJ = move.o list.o graph.o sight.o decode.o main.o
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
