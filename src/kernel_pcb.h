#ifndef _H_KERNEL_PCB
#define _H_KERNEL_PCB

#include <stdint.h>
#include "kernel_arm.h"

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

#define kernel_pcb_set_register(pcb, register, value) \
	( pcb ) -> mpSP [ register ] = ( uint32_t ) ( value )

#endif
