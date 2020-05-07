CC=gcc

CFLAGS=-c -Wall -Wextra

all: prog_stack.o compile link clean

compile: pdp11_wr.o pdp11_coms.o pdp11_run.o pdp.o

link:
	$(CC) pdp11_wr.o prog_stack.o pdp11_coms.o pdp11_run.o pdp.o -o pdp

prog_stack.o: prog_stack.c
	$(CC) $(CFLAGS) prog_stack.c

pdp11_wr.o: pdp11_wr.c
	$(CC) $(CFLAGS) pdp11_wr.c

pdp11_coms.o: pdp11_coms.c
	$(CC) $(CFLAGS) pdp11_coms.c

pdp11_run.o: pdp11_run.c
	$(CC) $(CFLAGS) pdp11_run.c

pdp.o: pdp.c
	$(CC) $(CFLAGS) pdp.c

clean:
	rm -rf pdp*.o