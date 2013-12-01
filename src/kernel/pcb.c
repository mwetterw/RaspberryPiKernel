#include "pcb.h"

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
