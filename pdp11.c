// pdp11.c

#include "pdp11.h"

void w_write(Adress adr, word w)
{
    assert(adr % 2 == 0);
    mem[adr] = w;
}

word w_read(Adress adr)
{
    assert(adr % 2 == 0);
    return mem[adr];
}
    
byte b_read(Adress adr)
{
    byte b;

    if (adr % 2 == 0)
        b = (byte)mem[adr];
    else
    {
        word w = mem[adr - 1];
        b = (byte)(w >> 8);
    }
    
    return b;
}

void b_write(Adress adr, byte b)
{
    if (adr % 2 == 0)
        mem[adr] = (word)b;
    else
    {
        mem[adr - 1] |= (word)(b << 8);
    }
}

void load_file(const char* filename)
{
    FILE* byte_code;
    byte_code = fopen(filename, "rb");

    Adress adr = 0;
    unsigned int num = 0;
    byte b = 0;

    while(fscanf(byte_code, "%04x %04x", &adr, &num) == 2)
    {
        for (unsigned int i = 0; i < num; i++)
        {
            fscanf(byte_code, "%02hhx ", &b);
            b_write(adr, b);
            adr++;
        }

        mem_dump(adr - num, num);

        adr = 0;
        num = 0;
    }

    fclose(byte_code);
}

void mem_dump(Adress adr, unsigned int num)
{
    for (unsigned int i = 0; i < num; i += 2)
    {
        trace("%06o : %06o\n", adr, w_read(adr));
        adr += 2;
    }
}

void trace(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

void reg_dump()
{
    for(int i = 0; i < 4; i++)
    {
        trace("r%d:%06o ", i, reg[i]);
    }
    trace("\n");
    for(int i = 5; i < 8; i++)
    {
        trace("r%d:%06o ", i, reg[i]);
    }
    trace("\n");
}