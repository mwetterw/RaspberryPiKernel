#ifndef _H_KERNEL_PCB_TURNSTILE
#define _H_KERNEL_PCB_TURNSTILE

#include "pcb.h"

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

/*
 * Remove PCB from turnstile
 * @param:
 * - PCB to remove ;
 * - Turnstile in which the deletion has to be done.
 */
void kernel_pcb_remove_turnstile ( kernel_pcb_t * pcb, kernel_pcb_turnstile_t * turnstile );

#endif
