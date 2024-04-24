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

struct Stack
{
    static constexpr int BYTES = 4096;
    uint32_t bytes[BYTES] __attribute__((aligned(16)));
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

static constexpr uint32_t HEAP_START = 2 * 1024 * 1024;
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

        // Debug::init(&uart);
        // Debug::debugAll = false;

        // Debug::printf("\n| What just happened? Why am I here?\n");
        printf("\n| What just happened? Why am I here?\n");

        // {
        //     Debug::printf("| Discovering my identity and features\n");
        //     cpuid_out out;
        //     cpuid(0, &out);

        //     Debug::printf("|     CPUID: ");
        //     auto one = [](uint32_t q)
        //     {
        //         for (int i = 0; i < 4; i++)
        //         {
        //             Debug::printf("%c", (char)q);
        //             q = q >> 8;
        //         }
        //     };
        //     one(out.b);
        //     one(out.d);
        //     one(out.c);
        //     Debug::printf("\n");

        //     cpuid(1, &out);
        //     if (out.c & 0x1)
        //     {
        //         Debug::printf("|     has SSE3\n");
        //     }
        //     if (out.c & 0x8)
        //     {
        //         Debug::printf("|     has MONITOR/MWAIT\n");
        //     }
        //     if (out.c & 0x80000000)
        //     {
        //         onHypervisor = true;
        //         Debug::printf("|     running on hypervisor\n");
        //     }
        //     else
        //     {
        //         onHypervisor = false;
        //         Debug::printf("|     running on physical hardware\n");
        //     }
        // }

        /* discover configuration */
        // configInit(&kConfig);
        // Debug::printf("| totalProcs %d\n", 4); // rasp pi has 4 cores
        // Debug::printf("| memSize 0x%x %dMB\n", // rasp pi has 1 gb = 1000MB
        //               1000,
        //               1000 / (1024 * 1024));

        printf("| totalProcs %d\n", 4); // rasp pi has 4 cores
        printf("| memSize 0x%x %dMB\n", // rasp pi has 1 gb = 1000MB
               1000,
               1000 / (1024 * 1024));
        // Debug::printf("| localAPIC %x\n", kConfig.localAPIC); // x86
        // Debug::printf("| ioAPIC %x\n", kConfig.ioAPIC);       // x86

        /* initialize the heap */
        heapInit((void *)HEAP_START, HEAP_SIZE); // kernel SP already moved, this is ok ?

        /* switch to dynamically allocated UART */
        // Debug::init(new U8250);
        //  Debug::printf("| switched to new UART\n");

        /* running global constructors */
        //  CRT::init(); // what is this

        // starting = new Barrier(kConfig.totalProcs);
        // stopping = new Barrier(kConfig.totalProcs);

        /* initialize LAPIC */
        // SMP::init(true);
        smpInitDone = true;

        /* initialize IDT */
        // IDT::init();

        // SMP::running.fetch_add(1);

        // The reset EIP has to be
        //     - divisible by 4K (required by LAPIC)
        //     - PPN must fit in 8 bits (required by LAPIC)
        //     - consistent with mbr.S
        // for (uint32_t id = 1; id < kConfig.totalProcs; id++)
        // {
        //     Debug::printf("| initialize %d\n", id);
        //     SMP::ipi(id, 0x4500);
        //     Debug::printf("| reset %d\n", id);
        //     Debug::printf("|      eip:0x%x\n", resetEIP);         // x86 instruction pointer. research arm equvialent ?
        //     SMP::ipi(id, 0x4600 | (((uintptr_t)resetEIP) >> 12)); // x86
        //     while (SMP::running <= id)
        //         ;
        // }
    }
    else
    {
        // SMP::running.fetch_add(1);
        // SMP::init(false);
    }

    // starting->sync();
    kernel_main();
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
