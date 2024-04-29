#include "arm/sysregs.h"

#include "mm.h"

.section ".text.boot"

.globl _start
_start:
    mrs    x0, mpidr_el1   // move values from mpidr to x0
    and    x0, x0,#0xFF    // Check processor id (aff0)
    cbz    x0, master      // core 0 will initialize the system
    b    proc_hang

proc_hang:  // spin all other cores
    // wfe
    b     proc_hang 

master:
    ldr    x0, =SCTLR_VALUE_MMU_DISABLED
    msr    sctlr_el1, x0 // we don't have virtual memory yet lol 

    ldr    x0, =HCR_VALUE // EL1 is AArch64. EL0 is determined by the register width 
    msr    hcr_el2, x0    // described in the current processing state when executing at EL0.

    ldr    x0, =SCR_VALUE  // processor is in non secure state
    msr    scr_el3, x0     // content program status reg is read write in non secure state
                      
    ldr    x0, =SPSR_VALUE // set values we want cpsr to be after eret
    msr    spsr_el3, x0   // CPSR can be restored after handling the

    adr    x0, el1_entry 
    msr    elr_el3, x0 // after exception, where do we want to return 

    eret

el1_entry:
    adr    x0, __bss_start // BSS is used for data that is declared but not initialized
    adr    x1, __bss_end
    sub    x1, x1, x0
    bl     memzero         // zero out BSS section


  mov    sp, #LOW_MEMORY
     // Call pickKernelStack to get the stack pointer value
    bl     pickKernelStack
    mov    sp, x0          // Set stack pointer to the value returned by pickKernelStack
    bl    kernelInit       // time to initialize kernel!
    b     proc_hang        // should never come here



proc_hang:  // spin all other cores
    wfe
    b     proc_hang 

     // Initialize and start secondary cores on Raspberry Pi 3
    ldr x1, =start_core_1    // Address to jump to for secondary cores
    ldr x2, =0xE0            // Mailbox for Core 1
    str x1, [x2]
	sev  

	ldr x1, =start_core_2    // Address to jump to for secondary cores
    ldr x2, =0xE8            // Mailbox for Core 2
    str x1, [x2]
	sev  
	
	//
    ldr x1, =start_core_3    // Address to jump to for secondary cores
    ldr x2, =0xF0            // Mailbox for Core 3
    str x1, [x2]
    sev                      // Send event to wake up all cores