@ vim: ft=arm
.globl kernel_arm_get_mode
kernel_arm_get_mode:
    mrs r0, cpsr
    and r0, r0, #0x1F
    bx lr

.globl kernel_arm_get_cpsr
kernel_arm_get_cpsr:
    mrs r0, cpsr
    bx lr


.globl ioread
ioread:
    ldr r0, [r0, +r1]
    bx lr

.globl ioreadi
ioreadi:
    ldr r0, [r0, +r1, LSL #2]
    bx lr


.globl iowrite
iowrite:
    str r2, [r0, +r1]
    bx lr

.globl iowritei
iowritei:
    str r3, [r0, +r1, LSL #2]
    bx lr



.globl cdelay
cdelay:
    subs r0, r0, #1
    bne cdelay
    bx lr
