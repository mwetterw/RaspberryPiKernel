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

.globl cdelay
cdelay:
    subs r0, r0, #1
    bne cdelay
    bx lr

.globl dmb
dmb:
	mov	r0, #0
	mcr	p15, 0, r0, c7, c10, 5
	mov pc, lr
