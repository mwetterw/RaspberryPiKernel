#ifndef _H_KERNEL_SCHEDULER
#define _H_KERNEL_SCHEDULER

#include "timer.h"
#include "config.h"

void kernel_scheduler_init ( );

void __attribute__ ( ( noreturn, naked ) ) kernel_scheduler_yield_noreturn ( );

#define kernel_scheduler_set_next_deadline() \
	kernel_timer_channel ( KERNEL_SCHEDULER_TIMER_CHANNEL ) = \
		kernel_timer_get_clock ( ) + KERNEL_SCHEDULER_TIMER_PERIOD; \
	kernel_timer_enable ( KERNEL_SCHEDULER_TIMER_CHANNEL )

#endif
