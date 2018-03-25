#include "pcb.h"
#include "pcb_turnstile.h"
#include "memory.h"
#include "config.h"
#include "scheduler.h"
#include "bcm2835/systimer.h"

static void pcb_bigbang ( void * ( * f ) ( void * ), void * args );

kernel_pcb_t * pcb_create ( void * f, void * args )
{
    kernel_pcb_t * pcb = memory_allocate ( sizeof ( kernel_pcb_t ) );
    pcb -> mpStack = memory_allocate ( KERNEL_STACK_SIZE );
    pcb -> mpSP = ( pcb -> mpStack ) + KERNEL_STACK_SIZE - 16;
    pcb_inherit_cpsr ( pcb );
    pcb_enable_irq ( pcb );
    pcb_set_register ( pcb, pc, pcb_bigbang );
    pcb_set_register ( pcb, r0, f );
    pcb_set_register ( pcb, r1, args );

    pcb_turnstile_pushback ( pcb, &turnstile_round_robin );

    return pcb;
}

void pcb_bigbang ( void * ( * f ) ( void * ), void * args )
{
    f ( args );

    irq_disable ( );
    pcb_turnstile_remove ( pcb_running, &turnstile_round_robin );
    memory_deallocate ( pcb_running -> mpStack );
    memory_deallocate ( pcb_running );

    scheduler_reschedule ( 0 );
}

void pcb_sleep ( kernel_pcb_t * pcb, uint32_t duration )
{
	pcb_turnstile_remove ( pcb, &turnstile_round_robin );
	pcb -> mWakeUpDate = systimer_get_clock ( ) + duration;
	pcb_turnstile_sorted_insert ( pcb, &turnstile_sleeping );

	if ( pcb == pcb_running )
	{
		scheduler_yield ( );
	}
}
