.globl kernel_arm_get_mode
kernel_arm_get_mode:
    mrs r0, cpsr
    and r0, r0, #0x1F
    bx lr

.globl kernel_arm_get_cpsr
kernel_arm_get_cpsr:
    mrs r0, cpsr
    bx lr

.globl kernel_arm_enable_irq
kernel_arm_enable_irq:
    cpsie i
	bx lr

.globl kernel_arm_disable_irq
kernel_arm_disable_irq:
    cpsid i
	bx lr

/*
.globl kernel_arm_enable_irq
kernel_arm_enable_irq:
    stmdb sp!, {r0}
	mrs r0, cpsr
	bic r0, r0, #0x80
	msr cpsr_c, r0
    ldmia sp!, {r0}
	bx lr

.globl kernel_arm_disable_irq
kernel_arm_disable_irq:
    stmdb sp!, {r0}
	mrs r0, cpsr
	orr r0, r0, #0x80
	msr cpsr_c, r0
    ldmia sp!, {r0}
	bx lr
*/
