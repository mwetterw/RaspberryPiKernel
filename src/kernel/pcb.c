#include "pcb.h"

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
