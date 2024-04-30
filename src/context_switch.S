.globl context_switch
context_switch:
    # x0 = save_old_sp_here
    # x1 = restore_new_sp_from_here
    str x19, [sp, #-8]!
    str x20, [sp, #-8]!
    str x21, [sp, #-8]!
    str x22, [sp, #-8]!
    str x23, [sp, #-8]!
    str x24, [sp, #-8]!
    str x25, [sp, #-8]!
    str x26, [sp, #-8]!
    str x27, [sp, #-8]!
    str x28, [sp, #-8]!
    str x29, [sp, #-8]!
    str x30, [sp, #-8]!
    # calle registers now saved, save the sp in designated area
    mov    x2, sp
    str    x2, [x0]
    # state now saved

    # load thread we are switching to sp
    mov    x2, x1
    mov    sp, x2

    ldr x30, [sp], 8
    ldr x29, [sp], 8
    ldr x28, [sp], 8
    ldr x27, [sp], 8
    ldr x26, [sp], 8
    ldr x25, [sp], 8
    ldr x24, [sp], 8
    ldr x23, [sp], 8
    ldr x22, [sp], 8
    ldr x21, [sp], 8
    ldr x20, [sp], 8
    ldr x19, [sp], 8
    # pop callee saved registers
    ret