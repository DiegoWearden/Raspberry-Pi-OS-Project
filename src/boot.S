#include "arm/sysregs.h"

#include "mm.h"

.section ".text.boot"

.globl _start
_start:
	mrs	x0, mpidr_el1		
	and	x0, x0,#0x3		// Check processor id
	cbz	x0, el1_entry		// Hang for all non-primary CPU
	b	proc_hang

proc_hang: 
    wfe
    mrs	x0, mpidr_el1
    and	x0, x0,#0x3
    cmp X0, #1
    b.eq start_core_1
    mrs	x0, mpidr_el1
    and	x0, x0,#0x3
    cmp X0, #2
    b.eq start_core_2
    mrs	x0, mpidr_el1
    and	x0, x0,#0x3
    cmp X0, #3
    b.eq start_core_3
    b proc_hang


el1_entry:
    adr    x0, bss_begin
    adr    x1, bss_end
    sub    x1, x1, x0
    bl     memzero

    mov    sp, #LOW_MEMORY
     // Initialize and start secondary cores on Raspberry Pi 3
    ldr x1, =start_core_1    // Address to jump to for secondary cores
    ldr x2, =0xE0           	 // Mailbox register for Core 1
    str x1, [x2]

	ldr x1, =start_core_2    // Address to jump to for secondary cores
    ldr x2, =0xE8                // Mailbox register for Core 2
    str x1, [x2]
	
	
    ldr x1, =start_core_3    // Address to jump to for secondary cores
    ldr x2, =0xF0                // Mailbox register for Core 3
    str x1, [x2]
    sev                           // Send event to wake up all cores
    bl    kernel_init
    b     proc_hang        // should never come here

start_core_1:
    ldr    x1, =LOW_MEMORY
    ldr    x2, =PAGE_SIZE
    add    sp, x1, x2          // sp = LOW_MEMORY + PAGE_SIZE
    bl     kernel_init
    b      start_core_1

start_core_2:
    ldr    x1, =LOW_MEMORY
    ldr    x2, =PAGE_SIZE
    add    sp, x1, x2, lsl #1  // sp = LOW_MEMORY + 2*PAGE_SIZE
    bl     kernel_init
    b      start_core_2

start_core_3:
    ldr    x1, =LOW_MEMORY
    ldr    x2, =PAGE_SIZE
    mov    x3, #3
    mul    x2, x2, x3          // x2 = 3 * PAGE_SIZE
    add    sp, x1, x2          // sp = LOW_MEMORY + 3*PAGE_SIZE
    bl     kernel_init
    b      start_core_3