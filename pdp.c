// pdp.c

#include "pdp11.h"

bool TRACE; // -t
bool BIGTRACE; // -T

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        help();
        return 0;
    }    
    
    if (argc > 1)
    {
        while (argc-- > 2)
        {
            switch (argv[argc][1])
            {
            case 't':
                TRACE = true;
                break;

            case 'T':
                TRACE = true;
                BIGTRACE = true;
                break;

            default:
                break;
            }
        }

        load_file(argv[1]);
    }

    run();

    return 0;
}


// describes work of pdp
void help()
{
    puts("Usage: ./pdp initial-core-file [options]\n\n"
    "Options:\n\t"
    "-t	show trace to stdout\n\t"
	"-T	show verbose trace to stdout\n");
}

// checks segmentation fault
int pdp_ok()
{
    if (pc > 0 && pc <= MEMSIZE)
        return 1;
    else
    {
        stack_delete(&PS);
        TRACE = true;
        trace("\n----------segmentation error----------\n");
        do_halt();
        return 0;
    }
}

// prints name of the func, adress of programm counter, function`s opcode 
// and some other things
void trace(const char *fmt, ...)
{
    if (TRACE == true)
    {
        va_list ap;
        va_start(ap, fmt);
        vprintf(fmt, ap);
        va_end(ap);
    }
}

// dumps registers after doing all operations
void big_trace()
{
    if (BIGTRACE == true)
        reg_dump();
}

// testing pdp working funcs
void test()
{
    // test1 -------------------
    Adress adr1 = 4;
    byte b11 = 0xcb;
    byte b01 = 0x0a;
    word w1 = 0xcb0a;
    w_write(adr1, w1);

    word b01res = b_read(adr1);
    word b11res = b_read(adr1 + 1);
    printf("ww/br \t %04x = %02x%02x\n", w1, b11res, b01res);
    //printf("%s\t%s\t%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    assert(b01 == b01res);
    assert(b11 == b11res);

    // test2 ---------------------
    Adress adr2 = 8;
    byte b12 = 0xac;
    byte b02 = 0xb1;
    word w2 = 0xacb1;
    b_write(adr2, b02);
    b_write(adr2 + 1, b12);

    w2 = w_read(adr2);
    word b02res = b_read(adr2);
    word b12res = b_read(adr2 + 1);
    printf("ww/br \t %04x = %02x%02x\n", w2, b12res, b02res);
    //printf("%s\t%s\t%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    assert(b02 == b02res);
    assert(b12 == b12res);
}