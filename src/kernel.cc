#include "kernel.h"
#include "mini_uart.h"
#include "printf.h"
#include "utils.h"

void putc(void *p, char c)
{
    if (c == '\n')
    {
        uart_send('\r');
    }
    uart_send(c);
}

extern "C" void kernel_main(void)
{
    uart_init();
    init_printf(0, putc);
    printf("Hello world: %d\r\n", 3);
    // printf("Exception level: %d\n", get_el());
    // printf("processor ID: %d\n", get_core_number());
    while (1)
    {
        uart_send(uart_recv());
    }
}