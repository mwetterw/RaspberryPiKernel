#define _C_KERNEL_SCHEDULER
#include "scheduler.h"
#include "bcm2835/systimer.h"
#include "bcm2835/pic.h"

kernel_pcb_t * kernel_pcb_running;
static kernel_pcb_t kernel_pcb_idle;

kernel_pcb_turnstile_t kernel_turnstile_round_robin;
kernel_pcb_turnstile_t kernel_turnstile_sleeping;

static void scheduler_elect ( );
static void __attribute__ ( ( noreturn ) ) kernel_idle_process ( );

extern void scheduler_ctxsw ( );


void scheduler_init ( )
{
	kernel_pcb_idle.mpStack = 0;
	kernel_pcb_idle.mpSP = ( void * ) 0x3000;
	kernel_pcb_set_register ( &kernel_pcb_idle, pc, ( uintptr_t ) kernel_idle_process );
	kernel_pcb_inherit_cpsr ( &kernel_pcb_idle );
	kernel_pcb_enable_irq ( &kernel_pcb_idle );

	kernel_pcb_turnstile_init ( &kernel_turnstile_round_robin );
	kernel_pcb_turnstile_init ( &kernel_turnstile_sleeping );

	kernel_pcb_running = 0;
}

void scheduler_handler ( uint32_t * oldSP )
{
    kernel_pcb_running -> mpSP = oldSP;
    scheduler_reschedule ( );
}

void scheduler_reschedule ( )
{
	scheduler_elect ( );
    systimer_update ( KERNEL_SCHEDULER_TIMER_PERIOD );
    scheduler_ctxsw ( kernel_pcb_running -> mpSP );
}

void scheduler_elect ( )
{
	// We wake up sleeping processes
	if ( kernel_turnstile_sleeping.mpFirst )
	{
		if
		(
			kernel_turnstile_sleeping.mpFirst -> mWakeUpDate <=
		    systimer_get_clock ( )
		)
		{
			kernel_pcb_t * current;
			current = kernel_pcb_turnstile_popfront ( &kernel_turnstile_sleeping );
			kernel_pcb_turnstile_pushback ( current, &kernel_turnstile_round_robin );
		}
	}

	if ( kernel_turnstile_round_robin.mpFirst )
	{
		kernel_pcb_running = kernel_turnstile_round_robin.mpFirst;
		kernel_pcb_turnstile_rotate ( &kernel_turnstile_round_robin );
		return;
	}

	kernel_pcb_running = &kernel_pcb_idle;
}

void kernel_idle_process ( )
{
    for ( ; ; )
    {
        pause ( );
    }
}
