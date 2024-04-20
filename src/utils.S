#include "init.h"

.globl put32
put32:
	str w1,[x0]
	ret

.globl get32
get32:
	ldr w0,[x0]
	ret

.globl delay
delay:
	subs x0, x0, #1
	bne delay
	ret

.globl get_el
	get_el:
	mrs x0, CurrentEL
	lsr x0, x0, #2
	ret

.global get_core_number
	get_core_number:
    mrs x0, MPIDR_EL1      // Read MPIDR_EL1 into x0
    and x0, x0, #0x3      // Check processor id
    ret                    // Return, with the result in x0 

.global monitor
	monitor:
	ret

.global atomic_exchange
.type atomic_exchange, %function
atomic_exchange:
    .cfi_startproc

    // Input:
    // x0 = address of the value to be exchanged
    // w1 = new value

    // Preserve registers that will be used
    sub     sp, sp, #16
    str     w1, [sp]         // Save new value locally

    // Load-Link/Store-Conditional Loop
.Lretry:
	bl 		print
    ldaxr   w2, [x0]         // Load from address and set exclusive
	
    ldr     w1, [sp]         // Re-load new value from stack
    stlxr   w3, w1, [x0]     // Attempt to store new value to address
    cbnz    w3, .Lretry      // If not zero, store failed, retry

    // Now w2 holds the original value at the address
    // Move the result to w0 (return value)
    mov     w0, w2

    // Restore the stack and return
    add     sp, sp, #16
    ret

    .cfi_endproc

