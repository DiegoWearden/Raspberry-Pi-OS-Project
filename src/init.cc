#include "init.h"
#include "mini_uart.h"
#include "printf.h"
#include "utils.h"
#include "critical.h"
#include "mm.h"

void putc(void *p, char c)
{
    if (c == '\n')
    {
        uart_send('\r');
    }
    uart_send(c);
}

static uint32_t counter = 0;

void work() {
    printf("*** hello %d\n",counter);
    printf("*** goodbye %d\n",counter);
    counter ++;
}

void kernelMain(void) {
    // The C way, use function pointers
    critical(work);
}

bool lockTaken = false;

extern "C" void uart_init_from_assembly(){
    uart_init();
    init_printf(0, putc);
}

extern "C" void kernel_init(void)
{   
    uart_init();
    init_printf(0, putc);
    int num = get_core_number();
    printf("Hello world: %d\r\n", num);
    kernelMain();

    // printf("Exception level: %d\n", get_el());
    // printf("processor ID: %d\n", get_core_number());
    while (1)
    {
        uart_send(uart_recv());
    }
}

extern "C" void print(){
    printf("in loop???\n");
}