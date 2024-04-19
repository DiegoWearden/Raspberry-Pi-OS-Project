#include "kernel.h"
#include "mini_uart.h"
#include "printf.h"
#include "utils.h"
#include "critical.h"

void putc(void *p, char c)
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
    uart_init();
    init_printf(0, putc);
    printf("Hello world: %d\r\n", 3);
    // printf("Exception level: %d\n", get_el());
    // printf("processor ID: %d\n", get_core_number());
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