// pdp11_commands.h

#ifndef PDP11_COMMANDS_H_
#define PDP11_COMMANDS_H_

#include "pdp11.h"

typedef struct
{
    word mask;
    word opcode;
    char* name;
    void (*do_func)(void);   
} Command;

Command cmd[] = {
    {0170000, 0010000, "mov", do_move},
    {0170000, 0060000, "add", do_add},
    {0111111, 0000000, "halt", do_halt},

    {0000000, 0000000, "unknown command", do_nothing}
};

enum coms {
    mov, // 0
    add, // 1
    halt, // 2
    unknwn // 3
};

#endif // PDP11_COMMANDS_H_
