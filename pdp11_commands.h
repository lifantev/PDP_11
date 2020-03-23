// pdp11_commands.h

#ifndef PDP11_COMMANDS_H_
#define PDP11_COMMANDS_H_

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
    {0777777, 0000000, "halt", do_halt},
    {0000000, 0000000, "unknown", do_nothing}
};

enum coms {
    mov, // 0
    add, // 1
    halt, // 2
    unknwn // 3
};

#endif // PDP11_COMMANDS_H_
