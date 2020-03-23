//pdp11_run.c

#include "pdp11.h"
#include "pdp11_commands.h"

void do_halt()
{
    trace("THE END\n");
    exit(EXIT_SUCCESS);
}

void do_move()
{

}

void do_add()
{

}

void do_nothing(){}

void run()
{
    pc = 01000;
    
    int itr = 0;
    while (cmd[itr].mask != cmd[unknwn].mask)
    {
        word w = w_read(pc);
        trace("%06o %06o : ", pc, w);
        pc += 2;
        
        itr = 0;
        while ((w & cmd[itr].mask) == cmd[itr].opcode)
        {
            trace(cmd[itr].name );
            cmd[itr].do_func();
        }
        itr = 0;

    }

}
