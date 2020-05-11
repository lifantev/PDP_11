// pdp11.h

#ifndef PDP11_H_
#define PDP11_H_

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#define MEMSIZE 64*1024

typedef unsigned char byte;
typedef unsigned int word;
typedef word Adress;


word mem[MEMSIZE];
#define ostat_adr 0177564
#define odata_adr 0177566

word reg[8];
#define pc reg[7]
#define sp reg[6]

typedef struct
{
    word val;
    word adr;
    word place;
} Arg;
Arg SS, DD;

word NN, R, XX;
bool BYTE;
enum PLACE {REG, MEM};
byte flag_N, flag_Z, flag_V, flag_C;



// pdp working funcs
void b_write(Adress, byte);
byte b_read(Adress);
void w_write(Adress, word);
word w_read(Adress);
void load_file(const char*);
void mem_dump(Adress, unsigned int);
void trace(const char*, ...);
void big_trace();
void test();
void reg_dump();
int pdp_ok();

// programm stack
#define SIZE 16
typedef word elem_t; 

struct Stack {
    bool zombie;
    size_t size;
    int cnt; 
    elem_t *Data;
};

struct Stack PS;

void stack_create(struct Stack*);
void stack_delete(struct Stack*);
void stack_push(struct Stack*, elem_t);
elem_t stack_pop(struct Stack*);

// pdp run funcs
Arg get_mr(word);
word get_nn(word);
word get_rl(word);
word get_rr(word);
word get_xx(word);
void run();
void do_mul();
void do_dec();
void do_rts();
void do_jsr();
void do_bpl();
void do_beq();
void do_br();
void do_tst();
void do_cmp();
void do_setn();
void do_setz();
void do_setv();
void do_setc();
void do_cln();
void do_clz();
void do_clv();
void do_clc();
void do_move();
void do_halt();
void do_clear();
void do_sob();
void do_add();
void do_nothing();

#endif // PDP11_H_
