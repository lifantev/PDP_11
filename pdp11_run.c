//pdp11_run.c

#include "pdp11.h"
#include "pdp11_commands.h"

int convert_value(word value)
{
    if ((value >> 8) == 0xFF)
        return -((~value + 1) & 0xFFFF);
    else 
        return value; 
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
                trace("#%d ", convert_value(res.val));
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

void do_halt()
{
    trace("\n----------halted----------\n");
    reg_dump();
    exit(EXIT_SUCCESS);
}

void do_move()
{
    DD.val = SS.val & 0xFFFF;

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

void do_sob()
{
    if (--reg[R] != 0)
    {
        pc = pc - NN * 2;
        run(pc, SPEC_CALL);
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
}

void do_add()
{
    DD.val = (SS.val + DD.val) & 0xFF;

    if (DD.place == REG)
        reg[DD.adr] = DD.val;
    else if (DD.place == MEM)
        w_write(DD.adr, DD.val);
}

void do_nothing(){}

void run(word program_counter, bool CALL)
{
    if(CALL == STAND_CALL)
        pc = 01000;
    else
        pc = program_counter;
    
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

        }

        trace(cmd[itr].name); 
        trace("\n");
        cmd[itr].do_func();
    }
}
