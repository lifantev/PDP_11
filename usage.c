// usage.c

#include "pdp11.h"

// for debug 
//#include "pdp11_commands.h"
//#include "pdp11.c"
//#include "pdp11_run.c"

void test();

int main(int argc, char* argv[])
{
    //if (argc > 1) 
    load_file("../gitrepo/tests/01_sum/sum_neg.o");

    //test();

    run();

    return 0;
}

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
