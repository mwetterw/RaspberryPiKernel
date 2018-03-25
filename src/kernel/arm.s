@ vim: ft=arm
.globl arm_get_mode
arm_get_mode:
    mrs r0, cpsr
    and r0, r0, #0x1F
    bx lr

.globl arm_get_cpsr
arm_get_cpsr:
    mrs r0, cpsr
    bx lr

.globl irq_enable
irq_enable:
    cpsie i
    bx lr

.globl irq_disable
irq_disable:
    mrs r0, cpsr
    cpsid i
    bx lr

.globl irq_restore
irq_restore:
    msr cpsr_c, r0
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

/* Put the processor into a low power consumption mode until an interrupt
 * occurs. This could have been implemented using "wfi" instruction.
 * Unfortunately, "wfi" is not supported before ARMv7. On ARMv6, depending on
 * the precise arch, "wfi" will either trigger an undefined instruction
 * exception or will just be implemented by a "nop" (ARMv6K and ARMv6T2). The
 * Raspberry Pi 1 uses an ARM1176-JZFS core which implements the ARMv6K
 * architecture. "wfi" would just do nothing here.  */
.macro wfi_
    mov r0, #0
    mcr p15, 0, r0, c7, c0, 4
.endm

/* Put the processor into a low power consumption mode until an interrupt
 * occurs.  If IRQ and FIQ are masked out by the CPU, any interrupt will still
 * wake the CPU up and execution continues at this "bx lr". If IRQ or FIQ are
 * enabled by the CPU, any interrupt will wake the CPU up and execution jumps
 * at the interrupt handler, with irq_lr or fiq_lr set at this "bx lr" + 4. */
.globl pause
pause:
    wfi_
    bx lr

/* Halt the CPU by going into deep sleep and trying to never be woken up again
 * by disabling all interrupts. Just in case, if we are woken up anyway, enter
 * deep sleep again and do this forever. */
.globl halt
halt:
    cpsid if
    bl pic_disable_all_interrupts
do_halt:
    wfi_

    @ This should never be reached.
    b do_halt
