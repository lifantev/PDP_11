// pdp_11.h

#ifndef PDP_11_H_
#define PDP_11_H_

#include <stdio.h>
#include <assert.h>

#define MEMSIZE 64*1024

typedef unsigned char byte;
typedef unsigned int word;
typedef word Adress;

word mem[MEMSIZE];

void b_write(Adress, byte);
byte b_read(Adress);
void w_write(Adress, word);
word w_read(Adress);
void load_file(const char*);
void mem_dump(Adress, unsigned int);

#endif // PDP_11_H_
