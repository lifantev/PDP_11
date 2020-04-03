// pdp11_commands.h

#ifndef PDP11_COMMANDS_H_
#define PDP11_COMMANDS_H_

enum PARAMS_MASK {
    NO_PARAMS, // 0
    HAS_DD, // 1
    HAS_SS, // 2
    HAS_SS_DD, // 3
    //
//    *new masks must be entered here*
    //
    PARAMS_MASK_SIZE // (last)
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
    {0170000, 0010000, "mov", do_move, HAS_SS_DD},
    {0170000, 0060000, "add", do_add, HAS_SS_DD},
    {0777777, 0000000, "halt", do_halt, NO_PARAMS},
    {0000000, 0000000, "unknown", do_nothing, NO_PARAMS}
};

enum COMS {
    MOV, // 0
    ADD, // 1
    HALT, // 2
    //
//    *new coms must be entered here*
    //
    UNKNWN // (last)
};

#endif // PDP11_COMMANDS_H_
