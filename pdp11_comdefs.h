// pdp11_comdefs.h

#ifndef PDP11_COMDEFS_H_
#define PDP11_COMDEFS_H_

// masks with command properties
enum PARAMS_MASK {
    NO_PARAMS = 0, 
    HAS_DD = 1, 
    HAS_SS = 2, 
    HAS_NN = 4,
    HAS_RL = 8,
    HAS_RR = 32,
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

extern Command cmd[];

/* commands that we have in our pdp
new commands should be added 
in the inicated place(in the same order as they are in the cmd[]) */
enum COMS {
    MOV,
    MOVB,
    ADD,
    HALT,
    CLRB,
    CLR,
    SOB,
    SETN,
    SETZ,
    SETV,
    SETC,
    CLN,
    CLZ,
    CLV,
    CLC,
    TST,
    TSTB,
    CMP,
    CMPB,
    BR,
    BEQ,
    BPL,
    JSR,
    RTS,
    DECB,
    DEC,
    MUL,
    //
//    *new coms must be entered here*
    //
    UNKNWN // (last)
};

#endif // PDP11_COMDEFS_H_
