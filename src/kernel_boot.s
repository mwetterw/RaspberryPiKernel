.globl _start
_start:
/* Exception vectors
 * doc_arm_specs A2-16, P.54/P.1138
 */
	ldr pc, reset_handler
	ldr pc, undefined_handler
	ldr pc, swi_handler
	ldr pc, prefetch_handler
	ldr pc, data_handler
	ldr pc, unused_handler
	ldr pc, irq_handler
	ldr pc, fiq_handler
reset_handler:      .word reset
undefined_handler:  .word undefined
swi_handler:        .word swi
prefetch_handler:   .word prefetch
data_handler:       .word data
unused_handler:     .word unused
irq_handler:        .word irq
fiq_handler:        .word fiq

reset:
	/* Also copy these handlers
	 * at address 0x0000 for compatibility
	 */
	mov r0, #0x8000
	mov r1, #0x0000
	ldmia r0!,{r2,r3,r4,r5,r6,r7,r8,r9}
	stmia r1!,{r2,r3,r4,r5,r6,r7,r8,r9}
	ldmia r0!,{r2,r3,r4,r5,r6,r7,r8,r9}
	stmia r1!,{r2,r3,r4,r5,r6,r7,r8,r9}

	;@ force the bit 0 (M) of the control register 1 to 0
	;@ doc_arm_specs.pdf chapter B3.4
	;@ This disables MMU.
	mrc p15, #0, r0, c1, c0, 0
	bic r0, #1
	mcr p15, #0, r0, c1, c0, 0


	mov sp, #0x8000000
	b kernel_main

undefined: b undefined
swi: b swi
prefetch: b prefetch
data: b data
unused: b unused
fiq: b fiq

irq: b irq
