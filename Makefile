CC=gcc

CFLAGS=-c -Wall -Wextra

all: pdp11

pdp11: pdp11.o pdp11_run.o usage.o
	$(CC) pdp11.o pdp11_run.o usage.o -o usage

pdp11.o: pdp11.c
	$(CC) $(CFLAGS) pdp11.c

pdp11_run.o: pdp11_run.c
	$(CC) $(CFLAGS) pdp11_run.c

usage.o: usage.c
	$(CC) $(CFLAGS) usage.c 

clean:
	rm -rf *.o pdp11