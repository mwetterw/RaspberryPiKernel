#ifndef _H_KERNEL_PCB
#define _H_KERNEL_PCB

#include <stdint.h>
#include "arm.h"

typedef struct kernel_pcb_s
{
	uint32_t * mpSP;
	uint32_t * mpStack;
	struct kernel_pcb_s * mpNext;
} kernel_pcb_t;

typedef struct kernel_pcb_turnstile_s
{
	kernel_pcb_t * mpFirst;
	kernel_pcb_t * mpLast;
} kernel_pcb_turnstile_t;

/*
 * Initializes a PCB turnstile
 * @param Turnstile to initialize
 */
void kernel_pcb_turnstile_init ( kernel_pcb_turnstile_t * turnstile );

/*
 * Adds a pcb to a turnstile.
 * @param PCB to add
 * @param Turnstile to add to
 */
void kernel_pcb_add_turnstile ( kernel_pcb_t * pcb, kernel_pcb_turnstile_t * turnstile );

/*
 * Lets the first PCB become the last,
 * the second the first, etc.
 * @param Turnstile to rotate
 */
void kernel_pcb_turnstile_rotate ( kernel_pcb_turnstile_t * turnstile );

#define r0 0
#define r1 1
#define r2 2
#define r3 3
#define r4 4
#define r5 5
#define r6 6
#define r7 7
#define r8 8
#define r9 9
#define r10 10
#define r11 11
#define r12 12
#define r14 13
#define r15 14
#define cpsr 15

#define lr r14
#define pc r15

#define kernel_pcb_set_register(pcb, register, value) \
	( pcb ) -> mpSP [ register ] = ( uint32_t ) ( value )

#define kernel_pcb_enable_irq(pcb) \
	( pcb ) -> mpSP [ cpsr ] &= ~( KERNEL_ARM_CPSR_IRQ_MASK )

#define kernel_pcb_disable_irq(pcb) \
	( pcb ) -> mpSP [ cpsr ] |= KERNEL_ARM_CPSR_IRQ_MASK

#define kernel_pcb_inherit_cpsr(pcb) \
	( pcb ) -> mpSP [ cpsr ] = kernel_arm_get_cpsr ( )

#endif
