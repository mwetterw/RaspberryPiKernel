#ifndef _H_PCB_TURNSTILE
#define _H_PCB_TURNSTILE

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
void pcb_turnstile_init ( kernel_pcb_turnstile_t * turnstile );

// Return whether a turnstile is empty or not
int pcb_turnstile_empty ( kernel_pcb_turnstile_t * turnstile );

/*
 * Adds a PCB to the end of a turnstile.
 * @param PCB to add
 * @param Turnstile to add to
 */
void pcb_turnstile_pushback ( kernel_pcb_t * pcb, kernel_pcb_turnstile_t * turnstile );

/*
 * Insert a PCB, sorted by mWakeUpDeadline
 * Nearest (lowest) deadline will be first.
 * @param PCB to add
 * @param Turnstile to add to
 */
void pcb_turnstile_sorted_insert ( kernel_pcb_t * pcb, kernel_pcb_turnstile_t * turnstile );

/*
 * Lets the first PCB become the last,
 * the second the first, etc.
 * @param Turnstile to rotate
 */
void pcb_turnstile_rotate ( kernel_pcb_turnstile_t * turnstile );

/*
 * Removes the first PCB of the list.
 * @param PCB to remove
 * @param Turnstile in which the deletion has to be done
 * @return pointer to removed PCB.
 */
kernel_pcb_t *
pcb_turnstile_popfront ( kernel_pcb_turnstile_t * turnstile );

/*
 * Remove PCB from turnstile
 * @param PCB to remove
 * @param Turnstile in which the deletion has to be done
 */
void pcb_turnstile_remove ( kernel_pcb_t * pcb, kernel_pcb_turnstile_t * turnstile );

#endif
