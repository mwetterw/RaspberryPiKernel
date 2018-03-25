#include "pcb_turnstile.h"

void pcb_turnstile_init ( kernel_pcb_turnstile_t * turnstile )
{
	turnstile -> mpFirst = 0;
	turnstile -> mpLast = 0;
}

int pcb_turnstile_empty ( kernel_pcb_turnstile_t * turnstile )
{
    return ( turnstile -> mpFirst == 0 );
}

void pcb_turnstile_pushback ( kernel_pcb_t * pcb, kernel_pcb_turnstile_t * turnstile )
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

void pcb_turnstile_sorted_insert ( kernel_pcb_t * pcb, kernel_pcb_turnstile_t * turnstile )
{
	// Empty turnstile
	if ( ! turnstile -> mpFirst )
	{
		turnstile -> mpFirst = pcb;
		turnstile -> mpLast = pcb;
		return;
	}

	// Push front
	if ( pcb -> mWakeUpDate <= turnstile -> mpFirst -> mWakeUpDate )
	{
		pcb -> mpNext = turnstile -> mpFirst;
		turnstile -> mpFirst = pcb;
		return;
	}

	// Push back
	if ( pcb -> mWakeUpDate >= turnstile -> mpLast -> mWakeUpDate )
	{
		turnstile -> mpLast -> mpNext = pcb;
		turnstile -> mpLast = pcb;
		pcb -> mpNext = 0;
		return;
	}

	kernel_pcb_t * * it;
	for
	(
			it = & ( turnstile -> mpFirst ) ;
			pcb -> mWakeUpDate > ( * it ) -> mWakeUpDate ;
			it = & ( ( * it ) -> mpNext )
	);
	pcb -> mpNext = * it;
	* it = pcb;
}

kernel_pcb_t *
pcb_turnstile_popfront ( kernel_pcb_turnstile_t * turnstile )
{
	if ( ! turnstile -> mpFirst )
	{
		return 0;
	}

	kernel_pcb_t * head = turnstile -> mpFirst;
	turnstile -> mpFirst = head -> mpNext;

	if ( turnstile -> mpLast == head )
	{
		turnstile -> mpLast = 0;
	}
	return head;
}

void pcb_turnstile_remove ( kernel_pcb_t * pcb, kernel_pcb_turnstile_t * turnstile )
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

void pcb_turnstile_rotate ( kernel_pcb_turnstile_t * turnstile )
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

