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

	;@ doc_arm_specs.pdf chapter B3.4.1, P.694/P.1138 & B4.9.2, P.740/P.1138
	;@ This disables MMU.
	;@ Sets the bit 0 of the c1 register of the CP15 coprocessor to 0
	;@ c1 is the control register of co-processor CP15.
	mrc p15, #0, r0, c1, c0, 0
	bic r0, #1
	mcr p15, #0, r0, c1, c0, 0

    ;@ Switch to IRQ Mode, disables IRQ & FIQ
	cpsid if, #0x12
	;@ Initializes IRQ stack pointer
    mov sp,#0x8000

    ;@ Switch to SUPERVISOR Mode
	cps #0x13
	;@ Initializes Supervisor stack pointer
    mov sp,#0x8000000

	b kernel_main

undefined: b undefined
swi: b swi
prefetch: b prefetch
data: b data
unused: b unused
fiq: b fiq

irq: b kernel_scheduler_handler


/*
 * CPSR
 * doc_arm_specs A2.5, P.49/P.1138
 *            31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
 *            N  Z  C  V  Q | RES  J | RESERVED  |  GE[3:0]  |    RESERVED     | E  A  I  F  T |    M[4:0]    |
 * Upon boot: 0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0   0  1  1  1  0  1  0  0  1  1
 * Upon boot:      4    |     0     |     0     |      0    |     0     |      1     |     D     |     3     |
 *
 * N  Negative
 * Z  Zero
 * C  Carry
 * V  oVerflow
 * Q  overflow DSP instruction
 * J  Current instruction set [ARM, Thumb, Jazelle] (together with T)
 * GE Greater or Equal (SIMD instruction)
 * E  Endianness (0 = Little Endian, 1 = Big Endian) @see setend instruction
 * A  Imprecise Data Aborts (1=off, 0=on)
 * I  IRQ (1=off, 0=on)
 * F  FIQ (1=off, 0=on)
 * T  Current instruction set [ARM, Thumb, Jazelle] (together with J)
 * M  Mode
 *
 *
 * MODE
 * 10000 16 0x10 User
 * 10001 17 0x11 FIQ
 * 10010 18 0x12 IRQ
 * 10011 19 0x13 Supervisor
 * 10111 23 0x17 Abort
 * 11011 27 0x1B Undefined
 * 11111 31 0x1F System
 *
 *
 * J   T   Instruction set
 * 0   0   ARM
 * 0   1   Thumb
 * 1   0   Jazelle
 * 1   1   RESERVED
 *
 *
 * MSR (http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0489c/Cihibbbh.html)
 * cpsr_c PSR[7:0]
 * cpsr_x PSR[15:8]
 * cpsr_s PSR[23:16]
 * cpsr_f PSR[31:24]
 */
