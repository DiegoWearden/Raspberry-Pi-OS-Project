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
    and x0, x0, #0xFF      // Check processor id
    ret                    // Return, with the result in x0 

.global monitor
	monitor:
	ret

.global atomic_exchange
atomic_exchange:
    // x0 = address of value
    // x1 = address of v (new value)

    // Load the new value from memory into x2
    LDR     w2, [x1]

loop:
    // Load the current value at address x0 with acquire semantics
    LDAXR   w3, [x0]
    
    // Try to store the new value x2 into address x0 with release semantics
    STLXR   w4, w2, [x0]
    
    // Check if the store succeeded, w4 = 0 means success
    CBNZ    w4, loop

    // If successful, the old value (x3) is in w3
    // Move the old value to x0 to return it
    MOV     x0, x3

    RET


