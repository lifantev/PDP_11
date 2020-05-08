//stack.c

#include "pdp11.h"

void stack_create(struct Stack *st)
{
    st->zombie = false;
    st->size = SIZE;
    st->cnt = 0;
    st->Data = (elem_t *)calloc(st->size, sizeof(st->Data[0]));
}

void stack_delete(struct Stack *st)
{
    if (st->zombie == true)
    {
        puts("Stack deleted");
        return;
    }

    st->zombie = true;
    st->size = 0;
    st->cnt = 0;
    free(st->Data);
}

void stack_push(struct Stack *st, elem_t elem)
{
    if (st->zombie == true)
    {
        puts("Stack deleted");
        return;
    }

    if (st->cnt == st->size)
    {
        st->size = 2 * st->size;
        st->Data = (elem_t *)realloc(st->Data, st->size * sizeof(st->Data[0]));
    }
    st->Data[st->cnt++] = elem;
}

elem_t stack_pop(struct Stack *st)
{
    if (st->zombie == true)
    {
        puts("Stack deleted");
        return 0;
    }

    elem_t res = 0;

    if (st->cnt == 0)
    {
        puts("Empty stack");
        return res;
    }

    st->cnt--;
    res = st->Data[st->cnt];
    st->Data[st->cnt + 1] = 0;

    if (st->size > 3 * st->cnt && st->size > SIZE)
    {
        st->size /= 2;
        st->Data = (elem_t *)realloc(st->Data, st->size * sizeof(st->Data[0]));
    }

    return res;
}