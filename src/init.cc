#include "init.h"
// #include "debug.h"
#include "printf.h"
// #include "config.h"
#include "mini_uart.h"
// #include "u8250.h"
#include "smp.h"
#include "utils.h"
#include "kernel.h"
#include "heap.h"
// #include "idt.h"
// #include "crt.h"
#include "stdint.h"
#include "atomic.h"
#include "threads.h"

struct Stack
{
    static constexpr int BYTES = 4096;
    uint64_t bytes[BYTES] __attribute__((aligned(16)));
};

void putc(void *p, char c)
{
    if (c == '\n')
    {
        uart_send('\r');
    }
    uart_send(c);
}

PerCPU<Stack> stacks; // kernel stacks are in the data section

static bool smpInitDone = false;

extern "C" uint64_t pickKernelStack(void)
{
    return (uint64_t)&stacks.forCPU(smpInitDone ? get_core_number() : 0).bytes[Stack::BYTES]; // if init done, get your stack, else we are all on core 0 stack
}

// static Barrier *starting = nullptr;
// static Barrier *stopping = nullptr;

// bool onHypervisor = true;

static constexpr uint32_t HEAP_START = 5 * 1024 * 1024;
static constexpr uint32_t HEAP_SIZE = 5 * 1024 * 1024;

extern "C" void kernelInit(void)
{

    // U8250 uart;

    if (!smpInitDone)
    {
        /*
        uart_init();
        uart_send_string("Hello, world!\n");
        init_printf(0, putc);

        from my kernel_main
        */

        uart_init();
        uart_send_string("Hello, world!\n");
        uart_send_string("uart initialized\n");
        init_printf(0, putc);
        printf("printf initialized!!!\n");
        printf("\n| What just happened? Why am I here?\n");

        /* discover configuration */
        printf("| totalProcs %d\n", 4); // rasp pi has 4 cores
        printf("| memSize 0x%x %dMB\n", // rasp pi has 1 gb = 1000MB
               1000,
               1000 / (1024 * 1024));

        /* initialize the heap */
        heapInit((void *)HEAP_START, HEAP_SIZE); // kernel SP already moved, this is ok

        // starting = new Barrier(kConfig.totalProcs); // 4
        // stopping = new Barrier(kConfig.totalProcs); // 4
        smpInitDone = true;

        /* initialize IDT */
        // IDT::init();

        // SMP::running.fetch_add(1);
        threadsInit();
    }
    else
    {
        // SMP::running.fetch_add(1);
        // SMP::init(false);
    }

    if (get_core_number() == 0)
    {
        thread([]
               {
                   printf("switching to main\n");
                   kernel_main();
                   // ::shutdown();
               });
    }
    // kernel_main();
    // starting->sync();
    stop();
    printf("fake shutdown");
    while (1)
    {
        uart_send(uart_recv());
    }
    // stopping->sync();
    // if (SMP::me() == 0)
    // {
    //     Debug::shutdown();
    // }
    // else
    // {
    //     while (true)
    //         asm volatile("hlt");
    // }
}
