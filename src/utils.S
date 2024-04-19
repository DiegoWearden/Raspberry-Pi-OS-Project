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
    and x0, x0, #0xff      // Check processor id
    ret                    // Return, with the result in x0 

.global monitor
	monitor:
	ret


.global atomic_exchange
atomic_exchange:
    // x0 = pointer to the target memory location (address of the data)
    // x1 = new value to store (address of the data containing new value)
    // x2 = pointer to store the old value (address where old data should be stored)

1:  ldaxr   w3, [x0]        // Load the value exclusively from the address in x0 into w3
    ldr     w4, [x1]        // Load new value from address in x1 into w4
    stlxr   w5, w4, [x0]    // Attempt to store w4 into the address in x0
    cbnz    w5, 1b          // If stlxr failed (w5 is not zero), retry the loop
    str     w3, [x2]        // Store the old value (w3) into the memory pointed by x2
    ret                     // Return from the function