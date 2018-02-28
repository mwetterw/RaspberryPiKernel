@ vim: ft=arm
.globl scheduler_ctxsw
scheduler_ctxsw:
    mov sp, r0
    ldmfd sp!, { r0 - r12, lr }
    rfefd sp!

.globl scheduler_yield
scheduler_yield:
    // Store cpsr to stack, but preserving original r0
    str r0, [sp, #-8 ]
    mrs r0, cpsr
    str r0, [sp, #-4]!
    ldr r0, [sp, #-4]

    stmfd sp!, { lr }
    stmfd sp!, { r0 - r12, lr }
    mov r0, sp

    b scheduler_reschedule
