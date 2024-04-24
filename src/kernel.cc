#include "kernel.h"
#include "mini_uart.h"
#include "printf.h"
#include "utils.h"
#include "critical.h"
#include "heap.h"

class Thing
{
    int me;

public:
    Thing() : me(0)
    {
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
    // uart_init();
    // uart_send_string("Hello, world!\n");
    // init_printf(0, putchar);

    printf("Hello world from the kernel!\n");
    printf("Exception level: %d\n", get_el());
    printf("processor ID: %d\n", get_core_number());
    printf("creating a struct\n");
    Thing *mything = new Thing;
    printf("thing created\n");
    printf("%d\n", mything->who());
    Thing *mything2 = new Thing;
    printf("thing2 created\n");
    printf("%d\n", mything2->who());

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