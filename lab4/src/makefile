CC=gcc
CFLAGS=-I.

all: process_memory parallel_min_max parallel_sum

process_memory: process_memory.c
	$(CC) -o process_memory process_memory.c $(CFLAGS)

parallel_min_max: utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

parallel_sum: utils.o sum.o utils.h sum.h
	$(CC) -o parallel_sum utils.o sum.o parallel_sum.c $(CFLAGS) -pthread

utils.o: utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o: utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

sum.o: sum.h
	$(CC) -o sum.o -c sum.c $(CFLAGS) -pthread

clean:
	rm -f utils.o find_min_max.o sum.o process_memory parallel_min_max parallel_sum

.PHONY: all clean