#include "pcb.h"
#include "memory.h"
#include "config.h"
#include "scheduler.h"

static void __attribute__ ( ( noreturn ) )
kernel_pcb_bigbang ( void * ( * f ) ( void * ), void * args );

static void
kernel_pcb_remove_turnstile ( kernel_pcb_t * pcb, kernel_pcb_turnstile_t * turnstile );

void kernel_pcb_turnstile_init ( kernel_pcb_turnstile_t * turnstile )
{
	turnstile -> mpFirst = 0;
	turnstile -> mpLast = 0;
}

void kernel_pcb_add_turnstile ( kernel_pcb_t * pcb, kernel_pcb_turnstile_t * turnstile )
{
	pcb -> mpNext = 0;

	if ( ! turnstile -> mpFirst )
	{
		turnstile -> mpFirst = pcb;
		turnstile -> mpLast = pcb;
		return;
	}

	turnstile -> mpLast -> mpNext = pcb;
	turnstile -> mpLast = pcb;
}

void kernel_pcb_remove_turnstile ( kernel_pcb_t * pcb, kernel_pcb_turnstile_t * turnstile )
{
    kernel_pcb_t * previous = 0;

    if ( turnstile -> mpFirst == pcb )
    {
        turnstile -> mpFirst = pcb -> mpNext;
    }
    else
    {
        // We get the previous pcb
        for ( previous = turnstile -> mpFirst ;
                previous->mpNext != pcb ;
                previous = previous -> mpNext );
        previous -> mpNext = pcb -> mpNext;
    }

    if ( turnstile -> mpLast == pcb )
    {
        turnstile -> mpLast = previous;
    }
}

void kernel_pcb_turnstile_rotate ( kernel_pcb_turnstile_t * turnstile )
{
	if ( ( ! turnstile -> mpFirst ) || turnstile -> mpFirst == turnstile -> mpLast )
	{
		return;
	}

	turnstile -> mpLast -> mpNext = turnstile -> mpFirst;
	turnstile -> mpLast = turnstile -> mpFirst;
	turnstile -> mpFirst = turnstile -> mpFirst -> mpNext;
	turnstile -> mpLast -> mpNext = 0;
}

kernel_pcb_t * kernel_pcb_create ( void * f, void * args )
{
    kernel_pcb_t * pcb = kernel_memory_allocate ( sizeof ( kernel_pcb_t ) );
    pcb -> mpStack = kernel_memory_allocate ( KERNEL_STACK_SIZE );
    pcb -> mpSP = ( pcb -> mpStack ) + KERNEL_STACK_SIZE - 16;
    kernel_pcb_inherit_cpsr ( pcb );
    kernel_pcb_enable_irq ( pcb );
    kernel_pcb_set_register ( pcb, pc, kernel_pcb_bigbang );
    kernel_pcb_set_register ( pcb, r0, f );
    kernel_pcb_set_register ( pcb, r1, args );

    kernel_pcb_add_turnstile ( pcb, &kernel_turnstile_round_robin );

    return pcb;
}

void kernel_pcb_bigbang ( void * ( * f ) ( void * ), void * args )
{
    f ( args );

    kernel_arm_disable_irq ( );
    kernel_pcb_remove_turnstile ( kernel_pcb_running, &kernel_turnstile_round_robin );
    kernel_memory_deallocate ( kernel_pcb_running -> mpStack );
    kernel_memory_deallocate ( kernel_pcb_running );

    kernel_scheduler_yield_noreturn ( );
}
