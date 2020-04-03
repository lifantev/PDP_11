//pdp11_run.c

#include "pdp11.h"
#include "pdp11_commands.h"

int convert_value(word value)
{
    if ((value >> 8) == 0xFF)
        return -((~value + 1) & 0xFF);
    else 
        return value; 
} 

Arg get_mr(word w)
{
    Arg res;
    int reg_num = w & 7;
    int mode = (w >> 3) & 7;

    switch (mode)
    {
        case 0:     // R3
            res.adr = reg_num;
            res.val = reg[reg_num];
            res.place = REG;
            trace("R%o ", reg_num);
            break;

        case 1:     // (R3)
            res.adr = reg[reg_num];
            res.place = REG;
            if (true)   // todo byte !!!!!!!!!!!!!!
                res.val = w_read(res.adr);
            else 
                res.val = b_read(res.adr);
            
            trace("(R%o) ", reg_num);
            break;

        case 2:     // (R3)+ or #3
            res.adr = reg[reg_num];
            res.place = REG;

            if (true) // todo byte !!!!!!!!!!!!!!!!
                res.val = w_read(res.adr);
            else
                res.val = b_read(res.adr);
            
            if (reg_num == 6 || 7)
            {
                reg[reg_num] += 2;
                trace("#%d ", convert_value(res.val));
            }
            else
            {
                if (true) // todo byte !!!!!!!!!!!!!!!!
                    reg[reg_num] += 2;
                else
                    reg[reg_num] += 1;

                trace("(R%o)+ ", reg_num);
            }
            break;
        
        case 3:
            res.adr = reg[reg_num];
            reg[reg_num] += 2;
            res.adr = w_read(res.adr); // todo byte ????????????
            res.val = w_read(res.adr);
            res.place = REG;

            if (reg_num == 6 || 7)
                trace("@3%o ", res.adr);
            else 
                trace("@(R%o)+ ", reg_num);
            break;
        
        case 4:
            res.place = REG;

            if (true)
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

        case 5:
            res.place = REG;
            reg[reg_num] -= 2;
            res.adr = reg[reg_num];
            res.adr = w_read(res.adr); // todo byte ???????????
            res.val = w_read(res.adr);
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
    trace("THE END\n");
    reg_dump();
    exit(EXIT_SUCCESS);
}

void do_move()
{
    DD.val = SS.val & 0xFF;

    if (DD.place == REG)
        reg[DD.adr] = DD.val;
    else if (DD.place == MEM)
        w_write(DD.adr, DD.val);
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
        
        if (cmd[itr].params != NO_PARAMS)
        {
            if ((cmd[itr].params & HAS_DD) == HAS_DD)
                DD = get_mr(w);

            if((cmd[itr].params & HAS_SS) == HAS_SS)
                SS = get_mr(w >> 6);
        }

        trace(cmd[itr].name); 
        printf("\n");
        cmd[itr].do_func();
    }
}
