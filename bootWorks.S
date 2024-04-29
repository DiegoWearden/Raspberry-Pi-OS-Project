#include "arm/sysregs.h"

#include "mm.h"

.section ".text.boot"

.globl _start
_start:
    mrs    x0, mpidr_el1
    and    x0, x0,#0xFF        // Check processor id
    cbz    x0, el1_entry        // Hang for all non-primary CPU
    b    proc_hang

proc_hang: 
    wfe
    b     proc_hang

master:
    ldr    x0, =SCTLR_VALUE_MMU_DISABLED
    msr    sctlr_el1, x0

    ldr    x0, =HCR_VALUE
    msr    hcr_el2, x0

    ldr    x0, =SCR_VALUE
    msr    scr_el3, x0

    ldr    x0, =SPSR_VALUE
    msr    spsr_el3, x0

    adr    x0, el1_entry
    msr    elr_el3, x0

el1_entry:
    adr    x0, __bss_start // BSS is used for data that is declared but not initialized
    adr    x1, __bss_end
    sub    x1, x1, x0
    bl     memzero // zero out BSS section

    mov    sp, #LOW_MEMORY

    bl    kernel_main // core zero calls kernel_init
    b     proc_hang        // should never come here

start_other_core:

	//mov sp, #LOW_MEMORY
	// set up stack per core
   // bl kernel_main
    b start_other_core

start_core_1:
    ldr    x1, =LOW_MEMORY
    ldr    x2, =PAGE_SIZE
    add    sp, x1, x2          // sp = LOW_MEMORY + PAGE_SIZE
    bl     kernel_main
    b      start_core_1

start_core_2:
    ldr    x1, =LOW_MEMORY
    ldr    x2, =PAGE_SIZE
    add    sp, x1, x2, lsl #1  // sp = LOW_MEMORY + 2*PAGE_SIZE
    bl     kernel_main
    b      start_core_2

start_core_3:
    ldr    x1, =LOW_MEMORY
    ldr    x2, =PAGE_SIZE
    mov    x3, #3
    mul    x2, x2, x3          // x2 = 3 * PAGE_SIZE
    add    sp, x1, x2          // sp = LOW_MEMORY + 3*PAGE_SIZE
    bl     kernel_main
    b      start_core_3