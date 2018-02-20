#ifndef _H_KERNEL_SCHEDULER
#define _H_KERNEL_SCHEDULER

#include "config.h"
#include "pcb.h"
#include "pcb_turnstile.h"

extern kernel_pcb_turnstile_t kernel_turnstile_round_robin;
extern kernel_pcb_turnstile_t kernel_turnstile_sleeping;

#ifndef _C_KERNEL_SCHEDULER
extern kernel_pcb_t * const kernel_pcb_running;
#endif

/*
 * Initializes kernel schedulers.
 * To be called once before the schedulers can be used.
 */
void scheduler_init ( );

void scheduler_handler ( uint32_t * oldSP );
void scheduler_reschedule ( );

extern void scheduler_yield ( );

#endif
