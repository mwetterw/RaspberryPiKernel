#ifndef _H_SCHEDULER
#define _H_SCHEDULER

#include "config.h"
#include "pcb.h"
#include "pcb_turnstile.h"

extern kernel_pcb_turnstile_t turnstile_round_robin;
extern kernel_pcb_turnstile_t turnstile_sleeping;

#ifndef _C_SCHEDULER
extern kernel_pcb_t * const pcb_running;
#endif

/*
 * Initializes kernel scheduler.
 * To be called once before the scheduler can be used.
 */
void scheduler_init ( );

void * scheduler_handler ( void * oldSP );

void scheduler_reschedule ( void * oldSP );

extern void scheduler_yield ( );

#endif
