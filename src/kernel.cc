#include "kernel.h"
#include "mini_uart.h"

extern "C" void kernel_main(void) {

    uart_send_string("Hello from core\r\n");

    while (1) {
        
    }
}