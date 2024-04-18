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