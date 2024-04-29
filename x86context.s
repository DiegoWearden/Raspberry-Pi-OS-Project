   .global context_switch

# implement a context switch
# store only callee registers
context_switch: 
    # take out our arguments from the stack and store in caller-saved registers
    # mov #offset(SRC), DST
    mov 4(%esp), %eax # eax = save_old_sp_here
    mov 8(%esp), %edx # edx = restore_new_sp_from_here

    # push regsiter values store callee-saved registers 
    # stack pointer automatically gets updated
    push %ebx 
    push %edi
    push %esi
    push %ebp

    # take new SP and move to correct place 
    mov %esp, (%eax)
    # our state is saved

    # restore other state
    mov %edx, %esp # sp = restore_new_sp_from_here
    # now i'm the other thread

    # pop from stack, new register values
    pop %ebp
    pop %esi
    pop %edi
    pop %ebx

    # return to address stored in the other stack
    # add a call to reenable interrupts ?
    ret 