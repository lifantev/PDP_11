//pdp11_coms.c

#include "pdp11.h"
#include "pdp11_comdefs.h"

void do_mul()
{
    if (SS.place == REG)
        SS.val = reg[SS.adr];

    if (DD.place == REG)
        DD.val = reg[DD.adr];

    SS.val = (SS.val * DD.val);

    // NZVC
    {
        if ((SS.val >> 15) == 1)
            do_setn();
        else
            do_cln();

        if (SS.val == 0)
            do_setz();
        else
            do_clz();

        if ((SS.val >> 16) == 1)
            do_setc();
        else
            do_clc();
    }

    SS.val &= 0xFFFF;
    
    if (SS.place == MEM)
        w_write(SS.adr, SS.val);
    else if (SS.place == REG)
        reg[SS.adr] = SS.val;

    trace(" =%06o", SS.val);
}

void do_dec()
{
    if (!BYTE)
        reg[R] -= 1;
    else
    {
        reg[R] = ((reg[R] & 0xFF) - 1) & 0xFFFF;
    }

    // NZVC
    {
        if ((reg[R] >> 15) == 1)
            do_setn();
        else
            do_cln();

        if (reg[R] == 0)
            do_setz();
        else
            do_clz();
    }

    trace(" =%06o", reg[R]);
}

void do_rts()
{
    pc = reg[R];
    sp = stack_pop(&PS);
    reg[R] = sp;
    trace(" %06o", reg[R]);
}

void do_jsr()
{
    sp = reg[R];
    stack_push(&PS, sp);
    reg[R] = pc;
    pc = DD.adr;
}

void do_bpl()
{
    if (flag_N == 0)
        do_br();
    else
        trace(" end");
}

void do_beq()
{
    if (flag_Z == 1)
        do_br();
    else
        trace(" end");
}

void do_br()
{
    pc = pc + 2 * XX;
    trace(" %06o\n", pc);
}

void do_tst()
{
    if (!BYTE)
    {
        if ((DD.val >> 15) == 1)
            do_setn();
        else
            do_cln();

        if (DD.val == 0)
            do_setz();
        else
            do_clz();
    }
    else
    {
        if (((DD.val & 0xFF) >> 7) == 1)
            do_setn();
        else
            do_cln();

        if ((DD.val & 0xFF) == 0)
            do_setz();
        else
            do_clz();
    }

    do_clv();
    do_clc();
}

void do_cmp()
{
    word cmp_val = 0;

    if (!BYTE)
    {
        cmp_val = (SS.val - DD.val) & 0x1FFFF;

        if ((cmp_val >> 15) == 1)
            do_setn();
        else
            do_cln();

        if (cmp_val == 0)
            do_setz();
        else
            do_clz();

        if ((cmp_val >> 16) == 1)
            do_setc();
        else
            do_clc();
    }
    else
    {
        cmp_val = (SS.val - DD.val) & 0x1FF;

        if ((cmp_val >> 7) == 1)
            do_setn();
        else
            do_cln();

        if (cmp_val == 0)
            do_setz();
        else
            do_clz();

        if ((cmp_val >> 8) == 1)
            do_setc();
        else
            do_clc();
    }
}

void do_setn()
{
    flag_N = 1;
}
void do_setz()
{
    flag_Z = 1;
}
void do_setv()
{
    flag_V = 1;
}
void do_setc()
{
    flag_C = 1;
}
void do_cln()
{
    flag_N = 0;
}
void do_clz()
{
    flag_Z = 0;
}
void do_clv()
{
    flag_V = 0;
}
void do_clc()
{
    flag_C = 0;
}

void do_halt()
{
    trace("\n----------halted----------\n");
    reg_dump();
    exit(EXIT_SUCCESS);
}

void do_move()
{
    DD.val = SS.val & 0xFFFF;
    
    // NZVC
    {
        if ((DD.val >> 15) == 1)
            do_setn();
        else
            do_cln();

        if (DD.val == 0)
            do_setz();
        else
            do_clz();

        do_clv();
    }

    // byte or word command
    {
        if (DD.adr == odata_adr)
            printf("%c", DD.val);

        if (!BYTE)
        {
            if (DD.place == REG)
                reg[DD.adr] = DD.val;
            else if (DD.place == MEM)
                w_write(DD.adr, DD.val);
        }
        else
        {
            if (DD.place == REG)
            {
                if (((DD.val & 0xFF) >> 7) == 0) // significant expansion
                    DD.val &= 0x00FF;
                else
                    DD.val = (DD.val & 0x00FF) | 0xFF00;

                reg[DD.adr] = DD.val;
            }
            else if (DD.place == MEM)
                b_write(DD.adr, DD.val);
        }
    }

    trace(" =%06o", DD.val);
}

void do_sob()
{
    if (--reg[R] != 0)
    {
        pc = pc - NN * 2;
    }
}

void do_clear()
{
    if (DD.place == REG)
        reg[DD.adr] = 0;
    else if (DD.place == MEM)
    {
        if (!BYTE)
            w_write(DD.adr, 0);
        else
            b_write(DD.adr, 0);
    }

    // NZVC
    {
        do_cln();
        do_setn();
        do_clv();
        do_clc();
    }
}

void do_add()
{
    DD.val = (SS.val + DD.val);

    // NZVC
    {
        if ((DD.val >> 15) == 1)
            do_setn();
        else
            do_cln();

        if (DD.val == 0)
            do_setz();
        else
            do_clz();

        if ((DD.val >> 16) == 1)
            do_setc();
        else
            do_clc();
    }

    DD.val &= 0xFFFF;
    trace(" =%06o", DD.val);

    // byte or word command
    {
        if (DD.place == REG)
            reg[DD.adr] = DD.val;
        else if (DD.place == MEM)
            w_write(DD.adr, DD.val);
    }
}

void do_nothing() {}