// pdp11_commands.h

#ifndef PDP11_COMMANDS_H_
#define PDP11_COMMANDS_H_

enum PARAMS_MASK {
    NO_PARAMS = 0, 
    HAS_DD = 1, 
    HAS_SS = 2, 
    HAS_NN = 4,
    HAS_R = 8,
    HAS_XX = 16,
};

typedef struct
{
    word mask;
    word opcode;
    char* name;
    void (*do_func)(void);   
    char params;
} Command;

Command cmd[] = {
    {0170000, 0010000, "mov", do_move, HAS_SS | HAS_DD},
    {0170000, 0110000, "movb", do_move, HAS_SS | HAS_DD},
    {0170000, 0060000, "add", do_add, HAS_SS | HAS_DD},
    {0177777, 0000000, "halt", do_halt, NO_PARAMS},
    {0177000, 0105000, "clrb", do_clear, HAS_DD},
    {0177000, 0005000, "clr", do_clear, HAS_DD},
    {0177000, 0077000, "sob", do_sob, HAS_R | HAS_NN},
    {0000000, 0000000, "unknown", do_nothing, NO_PARAMS}
};

enum COMS {
    MOV,
    MOVB,
    ADD,
    HALT,
    CLRB,
    CLR,
    //
//    *new coms must be entered here*
    //
    UNKNWN // (last)
};

#endif // PDP11_COMMANDS_H_
