CC=gcc

CFLAGS=-c -Wall -Wextra

all: compile link clean

compile: pdp11_wr.o pdp11_coms.o pdp11_run.o pdp.o

link:
	$(CC) pdp11_wr.o pdp11_coms.o pdp11_run.o pdp.o -o pdp

pdp11_wr.o: pdp11_wr.c
	$(CC) $(CFLAGS) pdp11_wr.c

pdp11_coms.o: pdp11_coms.c
	$(CC) $(CFLAGS) pdp11_coms.c

pdp11_run.o: pdp11_run.c
	$(CC) $(CFLAGS) pdp11_run.c

pdp.o: pdp.c
	$(CC) $(CFLAGS) pdp.c 

clean:
	rm -rf *.o pdp11