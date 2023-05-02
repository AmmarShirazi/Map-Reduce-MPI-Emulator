CC=mpicc
CFLAGS=-Wall -g

all: map-reduce-emu

map-reduce-emu: main.o nodemanager.o
	$(CC) $(CFLAGS) -o map-reduce-emu main.o nodemanager.o

file1.o: main.c
	$(CC) $(CFLAGS) -c main.c

file2.o: nodemanager.c
	$(CC) $(CFLAGS) -c nodemanager.c

clean:
	rm -f map-reduce-emu *.o
