// pdp11.h

#ifndef PDP11_H_
#define PDP11_H_

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#define MEMSIZE 64*1024

typedef unsigned char byte;
typedef unsigned int word;
typedef word Adress;


word mem[MEMSIZE];
word reg[8];
#define pc reg[7]


typedef struct
{
    word val;
    word adr;
    word place;
} Arg;
Arg SS, DD;

enum PLACE {REG, MEM};


void b_write(Adress, byte);
byte b_read(Adress);
void w_write(Adress, word);
word w_read(Adress);
void load_file(const char*);
void mem_dump(Adress, unsigned int);
void trace(const char*, ...);
void reg_dump();

Arg get_mr(word);
void run();
void do_move();
void do_halt();
void do_add();
void do_nothing();

#endif // PDP11_H_
