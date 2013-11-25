.globl kernel_pcb_inherit_cpsr
kernel_pcb_inherit_cpsr:
    stmdb sp!, {r0, r1, lr}
    bl kernel_arm_get_cpsr
    mov r1, r0
    ldr r0, [sp]
    ldr r0, [r0]
    str r1, [r0, #0x40]
    ldmia sp!, {r0, r1, lr}
    bx lr
