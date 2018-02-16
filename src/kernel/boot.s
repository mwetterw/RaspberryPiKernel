@ vim: ft=arm
@ NOTE: Keep r0, r1 & r2 unaltered throughout for kernel_main parameters
.globl _start
_start:
    @ Make sure everyone agree that the exception vector table is at 0x0!
    @ U-boot changes its location to 0x1bf5b000...
    mov r12, #0
    mcr p15, 0, r12, c12, c0, 0

    @ Copy our exception vector table to address 0, where ARM expect it to be
    ldr r11, =_vectors @ Store address of _vectors label somewhere and load it to r11 (pc relative)
    ldmia r11!, {r3-r10}
    stmia r12!, {r3-r10}
    @ Also copy our address constants just below the exception vector table
    @ This is needed because the jumps in _start will be pc relative
    ldmia r11!, {r3-r10}
    stmia r12!, {r3-r10}

_vectors:
    @ Set up the exception vector table (A2-16, P.54)
    @ Just jump to the address the label is pointing to
    ldr pc, reset_addr
    ldr pc, undefined_addr
    ldr pc, softirq_addr
    ldr pc, prefetch_addr
    ldr pc, data_addr
    ldr pc, unused_addr
    ldr pc, irq_addr
    ldr pc, fiq_addr

@ Create our own constants so that we always jump to the same absolute locations
reset_addr:     .word reset_handler
undefined_addr: .word undefined_handler
softirq_addr:   .word softirq_handler
prefetch_addr:  .word prefetch_handler
data_addr:      .word data_handler
unused_addr:    .word unused_handler
irq_addr:       .word irq_handler
fiq_addr:       .word fiq_handler


reset_handler:
    @ Switch to IRQ Mode, initialize IRQ stack pointer
    @ There's no need to disable IRQ/FIQ: entering reset already disabled them
    cps #0x12
    mov sp,#0x8000

    @ Switch to SUPERVISOR Mode, initialize SVC stack pointer
    cps #0x13
    mov sp,#0x8000000

    @ Launch our kernel!
    b kernel_main

irq_handler: b kernel_scheduler_handler

undefined_handler:
softirq_handler:
prefetch_handler:
data_handler:
unused_handler:
fiq_handler: b crash

crash: b crash
