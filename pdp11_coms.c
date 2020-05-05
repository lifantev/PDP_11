//pdp11_coms.c

#include "pdp11.h"
#include "pdp11_comdefs.h"

void do_beq()
{
    if (flag_Z == 1)
        do_br();
}

void do_br()
{
    XX = XX - 0400;
    pc = pc + 2 * XX;
}

void do_tst()
{
    if (!BYTE)
    {
        if ((DD.val >> 3) == 1) do_setn();
        else do_cln();
        
        if (((DD.val & 007) >> 3) == 1) do_setz();
        else do_clz();
    }
    else // whats the difference between them... idk
    {
        if ((DD.val >> 3) == 1) do_setn();
        else do_cln();
        
        if (((DD.val & 007) >> 3) == 1) do_setz();
        else do_clz();
    }
    
    do_clv(); do_clc();
}

void do_cmp()
{
    word cmp_val = 0;

    if (!BYTE)
    {
        cmp_val = (SS.val - DD.val) & 0xFFFF;

        if ((cmp_val >> 14) == 1) do_setn();
        else do_cln();

        if (cmp_val == 0) do_setz();
        else do_clz();

        if ((cmp_val >> 15) == 1) do_setc();
        else do_clc();
    }
    else // whats the difference between them... idk
    {
        cmp_val = (SS.val - DD.val) & 0xFFFF;

        if ((cmp_val >> 14) == 1) do_setn();
        else do_cln();

        if (cmp_val == 0) do_setz();
        else do_clz();

        if ((cmp_val >> 15) == 1) do_setc();
        else do_clc();
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
    if ((DD.val >> 14) == 1) do_setn();
    else do_cln();

    if (DD.val == 0) do_setz();
    else do_clz();

    do_clv();
    }

    // byte or word command
    {
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
        if(!BYTE)
            w_write(DD.adr, 0);
        else 
            b_write(DD.adr, 0);
    }

    // NZVC
    {
    do_cln(); do_setn(); do_clv(); do_clc();
    }
}

void do_add()
{
    DD.val = (SS.val + DD.val) & 0xFFFF;

    // NZVC
    {
    if ((DD.val >> 14) == 1) do_setn();
    else do_cln();

    if (DD.val == 0) do_setz();
    else do_clz();

    if ((DD.val >> 15) == 1) do_setc();
    else do_clc();
    }

    // byte or word command
    {
    if (DD.place == REG)
        reg[DD.adr] = DD.val;
    else if (DD.place == MEM)
        w_write(DD.adr, DD.val);
    }
}

void do_nothing(){}