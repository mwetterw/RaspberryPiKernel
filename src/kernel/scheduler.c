#define _C_SCHEDULER
#include "scheduler.h"
#include "bcm2835/systimer.h"

kernel_pcb_t * pcb_running;
static kernel_pcb_t pcb_idle;

kernel_pcb_turnstile_t turnstile_round_robin;
kernel_pcb_turnstile_t turnstile_sleeping;

static void scheduler_elect ( );
static void __attribute__ ( ( noreturn ) ) idle_process ( );

extern void scheduler_ctxsw ( );


void scheduler_init ( )
{
	pcb_idle.mpStack = 0;
	pcb_idle.mpSP = ( void * ) 0x3000;
	pcb_set_register ( &pcb_idle, pc, ( uintptr_t ) idle_process );
	pcb_inherit_cpsr ( &pcb_idle );
	pcb_enable_irq ( &pcb_idle );

	pcb_turnstile_init ( &turnstile_round_robin );
	pcb_turnstile_init ( &turnstile_sleeping );

	pcb_running = 0;
}

void * scheduler_handler ( void * oldSP )
{
    pcb_running -> mpSP = oldSP;
	scheduler_elect ( );
    systimer_update ( KERNEL_SCHEDULER_TIMER_PERIOD );

    return pcb_running -> mpSP;
}

void scheduler_reschedule ( void * oldSP )
{
    if ( oldSP )
    {
        pcb_running -> mpSP = oldSP;
    }

	scheduler_elect ( );
    systimer_update ( KERNEL_SCHEDULER_TIMER_PERIOD );
    scheduler_ctxsw ( pcb_running -> mpSP );
}

void scheduler_elect ( )
{
	// We wake up sleeping processes
	if ( turnstile_sleeping.mpFirst )
	{
        /* A simple timestamp comparison here would cause problems since the
         * clock overflows every 71min35sec. Only disadvantage is we can't
         * sleep more than 35min47sec. */
        if ( systimer_get_clock ( ) - turnstile_sleeping.mpFirst -> mWakeUpDate
                < 0x80000000UL )
		{
			kernel_pcb_t * current;
			current = pcb_turnstile_popfront ( &turnstile_sleeping );
			pcb_turnstile_pushback ( current, &turnstile_round_robin );
		}
	}

	if ( turnstile_round_robin.mpFirst )
	{
		pcb_running = turnstile_round_robin.mpFirst;
		pcb_turnstile_rotate ( &turnstile_round_robin );
		return;
	}

	pcb_running = &pcb_idle;
}

void idle_process ( )
{
    for ( ; ; )
    {
        pause ( );
    }
}
