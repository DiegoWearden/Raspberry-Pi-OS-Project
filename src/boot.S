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
    b proc_hang


el1_entry:
    adr    x0, bss_begin
    adr    x1, bss_end
    sub    x1, x1, x0
    bl     memzero
    mov   sp, #LOW_MEMORY
    bl    wake_up_cores
    bl    kernel_init
    b     proc_hang        // should never come here

secondary:
    mrs x0, mpidr_el1
    and x0, x0, #0xFF
    mov x1, #SECTION_SIZE
    mul x1, x1, x0
    add x1, x1, #LOW_MEMORY
    mov sp, x1
    bl kernel_init

wake_up_cores:
    mov x0, #0
    adr x0, secondary
    mov x1, #0xe0
    str x0, [x1]
    mov x1, #0xe8
    str x0, [x1]
    mov x1, #0xf0
    str x0, [x1]
    sev