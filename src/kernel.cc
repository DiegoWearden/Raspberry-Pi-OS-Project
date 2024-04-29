#include "kernel.h"
#include "mini_uart.h"
#include "printf.h"
#include "utils.h"
#include "critical.h"
#include "threads.h"

// -fno-rtti -fno-exceptions

class Foo
{
public:
    int rah;
    Foo() : rah(0)
    {
    }

    int getRah()
    {
        return rah;
    }

    void setRah(int x)
    {
        rah = x;
    }
};

class Thing
{
public:
    int me;
    Foo *theFoo;

public:
    Thing() : me(0)
    {
        // theFoo = new Foo();
    }
    int who()
    {
        return me;
    }
    int setWho(int x)
    {
        me = x;
        return me;
    }
};

void putchar(void *p, char c)
{
    if (c == '\n')
    {
        uart_send('\r');
    }
    uart_send(c);
}

static uint32_t counter = 0;

extern "C" void kernel_main(void)
{
    printf("Hello world from the kernel!\n");
    printf("Exception level: %d\n", get_el());
    printf("processor ID: %d\n", get_core_number());
    printf("creating a struct\n");
    Thing *mything = new Thing();
    printf("thing created\n");
    printf("%d\n", mything->who());
    // printf("calling foo inside of thing 1 %d\n", mything->theFoo->getRah());
    Thing *mything2 = new Thing();
    printf("thing2 created\n");
    // mything2->theFoo->setRah(1);
    printf("%d\n", mything2->who());
    // printf("calling foo inside of thing 2 %d\n", mything2->theFoo->getRah());

    critical([]
             {
        printf("*** hello %d\n",counter);
        printf("*** goodbye %d\n",counter);
    counter++; });
    while (1)
    {
        uart_send(uart_recv());
    }
}