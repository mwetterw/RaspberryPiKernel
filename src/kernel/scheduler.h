#ifndef _H_KERNEL_SCHEDULER
#define _H_KERNEL_SCHEDULER

#include "timer.h"
#include "config.h"
#include "pcb.h"

extern kernel_pcb_turnstile_t kernel_turnstile_round_robin;

#ifndef _C_KERNEL_SCHEDULER
extern kernel_pcb_t * const kernel_pcb_running;
#endif

/*
 * Initializes kernel schedulers.
 * To be called once before the schedulers can be used.
 */
void kernel_scheduler_init ( );


/*
 * Hands over control to the scheduler.
 * The current running process's context will be thrown away,
 * because we're not going to schedule it anymore.
 * ASSERT: IRQ already disabled before call.
 */
void __attribute__ ( ( noreturn, naked ) ) kernel_scheduler_yield_noreturn ( );


/*
 * Rearms timer and sets the next deadline for scheduling operation.
 */
#define kernel_scheduler_set_next_deadline() \
	kernel_timer_channel ( KERNEL_SCHEDULER_TIMER_CHANNEL ) = \
		kernel_timer_get_clock ( ) + KERNEL_SCHEDULER_TIMER_PERIOD; \
	kernel_timer_enable ( KERNEL_SCHEDULER_TIMER_CHANNEL )

#endif
