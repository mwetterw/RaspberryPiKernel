.globl kernel_arm_get_mode
kernel_arm_get_mode:
    mrs r0, cpsr
    and r0, r0, #0x1F
    bx lr

.globl kernel_arm_get_cpsr
kernel_arm_get_cpsr:
    mrs r0, cpsr
    bx lr
