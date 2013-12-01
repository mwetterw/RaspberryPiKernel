#ifndef _H_KERNEL_SCHEDULER
#define _H_KERNEL_SCHEDULER

#include "timer.h"
#include "config.h"
#include "pcb.h"

extern kernel_pcb_turnstile_t kernel_turnstile_round_robin;

#ifndef _C_KERNEL_SCHEDULER
extern kernel_pcb_t * const kernel_pcb_running;
#endif

void kernel_scheduler_init ( );

void __attribute__ ( ( noreturn, naked ) ) kernel_scheduler_yield_noreturn ( );

#define kernel_scheduler_set_next_deadline() \
	kernel_timer_channel ( KERNEL_SCHEDULER_TIMER_CHANNEL ) = \
		kernel_timer_get_clock ( ) + KERNEL_SCHEDULER_TIMER_PERIOD; \
	kernel_timer_enable ( KERNEL_SCHEDULER_TIMER_CHANNEL )

#endif
