@ vim: ft=arm
/* At "_start" lies the first instruction of our kernel.  If we run this code,
 * it means we are at address 0x8000.  We have no way of knowing what happened
 * before.  Who was running the CPU? Was it a kernel lauching us with kexec, a
 * bootloader like U-boot? Did they do their job correctly before calling us?
 * CPU state at reset is _known_. But we don't know what happened between CPU
 * reset pin assertion and jump to _start. Therefore we don't rely on
 * assumptions and ensure CPU state.  When running "reset_handler" however,
 * we can assume a known state.
 *
 * NOTE: We purposely avoid clobbering r0, r1 and r2 throughout to preserve
 * their values until kernel_main is called. */

.globl _start
_start:
    @ Make sure interrupts (IRQ and FIQ) are both disabled
    cpsid if

    @ Relocate the exception vector table to 0x0
    @ U-boot changes its location to 0x1bf5b000...
    @ Any interrupt would just jump back to dead bootloader otherwise!
    mov r12, #0
    mcr p15, 0, r12, c12, c0, 0

    @ Copy our exception vector table to 0x0, where ARM expect it to be
    @ Then, also copy the constants just below the exception vector table
    @ This is needed because the jumps in "_vectors" will be pc relative
    ldr r11, =_vectors
    ldmia r11!, {r3-r10}
    stmia r12!, {r3-r10}
    ldmia r11!, {r3-r10}
    stmia r12!, {r3-r10}

    @ Disable data cache and MMU at the same time
    mrc p15, 0, r11, c1, c0, 0
    bic r11, r11, #5
    mcr p15, 0, r11, c1, c0, 0

_vectors:
    @ Set up the exception vector table
    @ There is just enough space for one instruction. So let's jump!
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


/* This handler is called by "_start" and may also be called directly if
 * a reset exception occurs, where the CPU state will be known.
 * NOTE: Purposely jumping to this handler at 0x0 wouldn't reset the CPU... */
reset_handler:
    @ Switch to IRQ Mode, initialize IRQ stack pointer
    cps #0x12
    mov sp,#0x8000

    @ Switch to SUPERVISOR Mode, initialize SVC stack pointer
    cps #0x13
    mov sp,#0x8000000

    @ Zero whole BSS section
    ldr r3, =_bss_start
    ldr r4, =_bss_end
    mov r5, #0
    mov r6, #0
    mov r7, #0
    mov r8, #0
bss_zero_loop:
    cmp r3, r4
    stmloia r3!, {r5-r8}
    blo bss_zero_loop

    @ Set kernel heap start address just after BSS
    ldr r3, =kernel_memory_heap
    ldr r4, =_end
    str r4, [r3]

    @ Launch our kernel!
    b kernel_main


undefined_handler:
softirq_handler:
prefetch_handler:
data_handler:
unused_handler:
fiq_handler: b crash
