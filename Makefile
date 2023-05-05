CC=mpicc
CFLAGS=-Wall -g

all: map-reduce-emu

map-reduce-emu: main.o node_manager.o matrix_formatter.o
	$(CC) $(CFLAGS) -o map-reduce-emu main.o node_manager.o matrix_formatter.o

file1.o: main.c
	$(CC) $(CFLAGS) -c main.c

file2.o: node_manager.c
	$(CC) $(CFLAGS) -c node_manager.c

	
file3.o: matrix_formatter.c
	$(CC) $(CFLAGS) -c matrix_formatter.c

clean:
	rm -f map-reduce-emu *.o
