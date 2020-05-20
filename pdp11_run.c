//pdp11_run.c

#include "pdp11.h"
#include "pdp11_comdefs.h"

word mem[MEMSIZE];
word reg[8];
#define pc reg[7]
#define sp reg[6]

Command cmd[] = {
    {0170000, 0010000, "mov", do_move, HAS_SS | HAS_DD},
    {0170000, 0110000, "movb", do_move, HAS_SS | HAS_DD},
    {0170000, 0060000, "add", do_add, HAS_SS | HAS_DD},
    {0177777, 0000000, "halt", do_halt, NO_PARAMS},
    {0177700, 0105000, "clrb", do_clear, HAS_DD},
    {0177700, 0005000, "clr", do_clear, HAS_DD},
    {0177000, 0077000, "sob", do_sob, HAS_RL | HAS_NN},
    {0000777, 0000270, "setn", do_setn, NO_PARAMS},
    {0000777, 0000264, "setz", do_setz, NO_PARAMS},
    {0000777, 0000262, "setv", do_setv, NO_PARAMS},
    {0000777, 0000261, "setc", do_setc, NO_PARAMS},
    {0000777, 0000250, "cln", do_cln, NO_PARAMS},
    {0000777, 0000244, "clz", do_clz, NO_PARAMS},
    {0000777, 0000242, "clv", do_clv, NO_PARAMS},
    {0000777, 0000241, "clc", do_clc, NO_PARAMS},
    {0177700, 0005700, "tst", do_tst, HAS_DD},
    {0177700, 0105700, "tstb", do_tst, HAS_DD},
    {0170000, 0020000, "cmp", do_cmp, HAS_DD | HAS_SS},
    {0170000, 0120000, "cmpb", do_cmp, HAS_DD | HAS_SS},
    {0077400, 0000400, "br", do_br, HAS_XX},
    {0077400, 0001400, "beq", do_beq, HAS_XX},
    {0100000, 0100000, "bpl", do_bpl, HAS_XX},
    {0177000, 0004000, "jsr", do_jsr, HAS_RL | HAS_DD},
    {0177770, 0000200, "rts", do_rts, HAS_RR},
    {0177700, 0105300, "decb", do_dec, HAS_RR},
    {0177700, 0005300, "dec", do_dec, HAS_RR},
    {0170000, 0070000, "mul", do_mul, HAS_DD | HAS_SS},
    {0000000, 0000000, "unknown", do_nothing, NO_PARAMS}};

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

    if (res < 0200)
        return res;
    else
        return res - 0400;
}

word get_nn(word w)
{
    word res = w & 15;
    trace("%06o ", pc - NN * 2);
    return res;
}

word get_rl(word w)
{
    word res = ((w >> 6) & 7);
    return res;
}

word get_rr(word w)
{
    word res = (w & 7);
    return res;
}

Arg get_mr(word w)
{
    Arg res;
    int reg_num = w & 7;
    int mode = (w >> 3) & 7;

    switch (mode)
    {
    case 0: // Rn
    {
        res.adr = reg_num;
        res.val = reg[reg_num];
        res.place = REG;
        trace("R%o ", reg_num);
        break;
    }
    case 1: // (Rn)
    {
        res.adr = reg[reg_num];
        res.place = MEM;
        if (!BYTE)
            res.val = w_read(res.adr);
        else
            res.val = b_read(res.adr);

        trace("(R%o) ", reg_num);
        break;
    }
    case 2: // (Rn)+ or #n
    {
        res.adr = reg[reg_num];

        if (!BYTE)
            res.val = w_read(res.adr);
        else
            res.val = b_read(res.adr);

        if (reg_num == 6 || reg_num == 7)
        {
            res.place = MEM;
            reg[reg_num] += 2;
            trace("#%06o ", res.val);
        }
        else
        {
            res.place = REG;

            if (!BYTE)
                reg[reg_num] += 2;
            else
                reg[reg_num] += 1;

            trace("(R%o)+ ", reg_num);
        }
        break;
    }
    case 3: // @(Rn)+ or @#n
    {
        res.adr = reg[reg_num];
        reg[reg_num] += 2;
        res.adr = w_read(res.adr);

        if (!BYTE)
            res.val = w_read(res.adr);
        else
            res.val = b_read(res.adr);

        res.place = MEM;

        if (reg_num == 6 || 7)
        {
            if (res.adr == odata_adr)
                trace("@#odata ");
            else if (res.adr == ostat_adr)
                trace("@#ostat ");
            else
                trace("@#%06o ", res.adr);
        }
        else
            trace("@(R%o)+ ", reg_num);
        break;
    }
    case 4: // -(Rn)
    {
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
    }
    case 5: // @-(Rn)
    {
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
    }
    case 6: // x(Rn) or xx
    {
        res.place = MEM;

        word displacement = w_read(pc);
        pc += 2;

        if (reg_num != 7)
        {
            if (displacement > 0100000)
                displacement = displacement - 0200000;
        }

        res.adr = reg[reg_num] + displacement;

        if ((reg_num == 7) && (res.adr != odata_adr) && (displacement > 0100000))
            res.adr -= 0200000; 
            
        if (!BYTE)
            res.val = w_read(res.adr);
        else
            res.val = b_read(res.adr);

        if (reg_num == 7)
        {
            if (res.adr == odata_adr)
                trace("odata ");
            else if (res.adr == ostat_adr)
                trace("ostat ");
            else
                trace("%06o ", displacement);
        }
        else
            trace("%06o(R%o) ", displacement & 0xFFFF, reg_num);
        break;
    }
    default:
        fprintf(stderr, "Mode %o not implemented.", mode);
        exit(EXIT_FAILURE);
    }

    return res;
}

void run()
{
    trace("\n----------running----------\n");

    pc = 01000;
    w_write(ostat_adr, 0200); // constant display status

    int itr = 0;
    while (cmd[itr].mask != cmd[HALT].mask && pdp_ok())
    {
        word w = w_read(pc);
        trace("\n%06o %06o : ", pc, w);
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
            if ((cmd[itr].params & HAS_RL) == HAS_RL)
                R = get_rl(w);

            if ((cmd[itr].params & HAS_RR) == HAS_RR)
                R = get_rr(w);

            if ((cmd[itr].params & HAS_NN) == HAS_NN)
                NN = get_nn(w);

            if ((cmd[itr].params & HAS_SS) == HAS_SS)
                SS = get_mr(w >> 6);

            if ((cmd[itr].params & HAS_DD) == HAS_DD)
                DD = get_mr(w);

            if ((cmd[itr].params & HAS_XX) == HAS_XX)
                XX = get_xx(w);
        }

        trace(cmd[itr].name);
        cmd[itr].do_func();   
        big_trace();
    }
}
