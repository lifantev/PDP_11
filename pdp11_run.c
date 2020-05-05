//pdp11_run.c

#include "pdp11.h"
#include "pdp11_comdefs.h"

Command cmd[] = {
    {0170000, 0010000, "mov", do_move, HAS_SS | HAS_DD},
    {0170000, 0110000, "movb", do_move, HAS_SS | HAS_DD},
    {0170000, 0060000, "add", do_add, HAS_SS | HAS_DD},
    {0177777, 0000000, "halt", do_halt, NO_PARAMS},
    {0177000, 0105000, "clrb", do_clear, HAS_DD},
    {0177000, 0005000, "clr", do_clear, HAS_DD},
    {0177000, 0077000, "sob", do_sob, HAS_R | HAS_NN},
    {0000277, 0000270, "sen", do_setn, NO_PARAMS},
    {0000277, 0000264, "sez", do_setz, NO_PARAMS},
    {0000277, 0000262, "sev", do_setv, NO_PARAMS},
    {0000277, 0000261, "sec", do_setc, NO_PARAMS},
    {0000277, 0000250, "cln", do_cln, NO_PARAMS},
    {0000277, 0000244, "clz", do_clz, NO_PARAMS},
    {0000277, 0000242, "clv", do_clv, NO_PARAMS},
    {0000277, 0000241, "clc", do_clc, NO_PARAMS},
    {0105700, 0005700, "tst", do_tst, HAS_DD},
    {0105700, 0105700, "tstb", do_tst, HAS_DD},
    {0120000, 0020000, "cmp", do_cmp, HAS_DD | HAS_SS},
    {0120000, 0120000, "cmpb", do_cmp, HAS_DD | HAS_SS},
    {0077400, 0000400, "br", do_br, HAS_XX},
    {0077400, 0001400, "beq", do_beq, HAS_XX},
    {0000000, 0000000, "unknown", do_nothing, NO_PARAMS}
};

int convert_value(word value)
{
    if ((value >> 8) == 0xFF)
        return -((~value + 1) & 0xFFFF);
    else 
        return value; 
} 

word get_xx(word w)
{
    word res = w & 0xFF;
    return res;
}

word get_nn(word w)
{
    word res = w & 15;
    trace("%06o ", pc - NN * 2);    
    return res;
}

word get_r(word w)
{
    word res = ((w >> 6) & 7);
    trace("R%o ", res);
    return res;
}

Arg get_mr(word w)
{
    Arg res;
    int reg_num = w & 7;
    int mode = (w >> 3) & 7;

    switch (mode)
    {
        case 0:     // Rn
            res.adr = reg_num;
            res.val = reg[reg_num];
            res.place = REG;
            trace("R%o ", reg_num);
            break;

        case 1:     // (Rn)
            res.adr = reg[reg_num];
            res.place = REG;
            if (!BYTE)
                res.val = w_read(res.adr);
            else 
                res.val = b_read(res.adr);
            
            trace("(R%o) ", reg_num);
            break;

        case 2:     // (Rn)+ or #n
            res.adr = reg[reg_num];
            res.place = REG;

            if (!BYTE)
                res.val = w_read(res.adr);
            else
                res.val = b_read(res.adr);
            
            if (reg_num == 6 || reg_num == 7)
            {
                reg[reg_num] += 2;
                trace("#%o ", res.val);
            }
            else
            {
                if (!BYTE)
                    reg[reg_num] += 2;
                else
                    reg[reg_num] += 1;

                trace("(R%o)+ ", reg_num);
            }
            break;
        
        case 3:     // @(Rn)+ or @#n
            res.adr = reg[reg_num];
            reg[reg_num] += 2;
            res.adr = w_read(res.adr);

            if (!BYTE)
                res.val = w_read(res.adr);
            else
                res.val = b_read(res.adr);
                
            res.place = REG;

            if (reg_num == 6 || 7)
                trace("@#%o ", res.adr);
            else 
                trace("@(R%o)+ ", reg_num);
            break;
        
        case 4:     // -(Rn)
            res.place = MEM;

            if (!BYTE)
            {   
                reg[reg_num] -= 2;
                res.adr = reg[reg_num];
                res.val = w_read(res.adr);
            }
            else
            {
                if (reg_num == 6 || 7)
                    reg[reg_num] -= 2;
                else
                    reg[reg_num] -= 1;

                res.adr = reg[reg_num];
                res.val = b_read(res.adr);
            }
            trace("-(R%o) ", reg_num);
            break;

        case 5:     // @-(Rn)
            res.place = REG;
            reg[reg_num] -= 2;
            res.adr = reg[reg_num];
            res.adr = w_read(res.adr);

            if (!BYTE)
                res.val = w_read(res.adr);
            else
                res.val = b_read(res.adr);

            trace("@-(R%o) ", reg_num);
            break;

        default:
            fprintf(stderr, "Mode %o not implemented.", mode);
            exit(EXIT_FAILURE);
    }

    return res;
}

void run()
{
    pc = 01000;
    
    int itr = 0;
    while (cmd[itr].mask != cmd[HALT].mask)
    {
        word w = w_read(pc);
        trace("%06o %06o : ", pc, w);
        pc += 2;
        
        itr = 0;
        while ((w & cmd[itr].mask) != cmd[itr].opcode) 
            itr++;

        if ((cmd[itr].opcode >> 15) == 1) // byte operation or not
            BYTE = true;
        else if ((cmd[itr].opcode >> 15) == 0)
            BYTE = false;
        
        if (cmd[itr].params != NO_PARAMS)
        {
            if((cmd[itr].params & HAS_R) == HAS_R)
                R = get_r(w);

            if((cmd[itr].params & HAS_NN) == HAS_NN)
                NN = get_nn(w);

            if((cmd[itr].params & HAS_SS) == HAS_SS)
                SS = get_mr(w >> 6);

            if ((cmd[itr].params & HAS_DD) == HAS_DD)
                DD = get_mr(w);
                
            if ((cmd[itr].params & HAS_XX) == HAS_XX)
                XX = get_xx(w);

        }

        trace(cmd[itr].name); 
        trace("\n");
        cmd[itr].do_func();
    }
}
