#include "init.h"
#include "mini_uart.h"
#include "printf.h"
#include "utils.h"
#include "critical.h"
#include "mm.h"
#include "heap.h"
#include "threads.h"
#include "percpu.h"


static constexpr uint32_t HEAP_START = 1 * 1024 * 1024;
static constexpr uint32_t HEAP_SIZE = 256 * 1024 * 1024;

void putc(void *p, char c)
{
    if (c == '\n')
    {
        uart_send('\r');
    }
    uart_send(c);
}

struct Stack {
    static constexpr int BYTES = 4096;
    uint64_t bytes[BYTES] __attribute__ ((aligned(16)));
};

PerCPU<Stack> stacks;

static bool smpInitDone = false;

extern "C" uint64_t pickKernelStack(void) {
    return (uint64_t) &stacks.forCPU(smpInitDone ? get_core_number() : 0).bytes[Stack::BYTES];
}

static uint32_t counter = 0;

void work() {
    printf("*** hello %d\n",counter);
    printf("*** goodbye %d\n",counter);
    counter ++;
}

void kernelMain_t1(void) {

    int waitingFor = 5;

    for (int i=0; i<4; i++) {
        printf("creating thread %d\n", i);
        thread([&waitingFor] {
            waitingFor--;
            while (waitingFor != 0) yield();
        });
    }

    waitingFor--;
    while (waitingFor != 0) {
        yield();
    }

    printf("*** x = %d\n",waitingFor);

}

extern "C" void uart_init_from_assembly(){
    uart_init();
    init_printf(0, putc);
}

extern "C" void kernel_init(void)
{   
    uart_init();
    init_printf(0, putc);
    int num = get_core_number();
    printf("\n\ncore number: %d\r\n", num);
    printf("Exception level: %d\n", get_el());
    heapInit((void*)HEAP_START, HEAP_SIZE);
    threadsInit();
    kernelMain_t1();
    void* thing = malloc(sizeof(int));
    printf("sizeof(thing): %x", thing);
    char *string = new char[sizeof(int)];
    printf("sizeof(string): %x", string);
    delete thing;
    // printf("processor ID: %d\n", get_core_number());
    while (1)
    {
        uart_send(uart_recv());
    }
}

extern "C" void print(){
    printf("in loop???\n");
}