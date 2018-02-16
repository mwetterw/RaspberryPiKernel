#include "systimer.h"
#include "pic.h"

static volatile struct systimer * systimer =
    ( volatile struct systimer * ) SYSTIMER_BASE;

void systimer_init ( )
{
    pic_enable_irq ( IRQ_TIMER1 );
}

uint32_t systimer_get_clock ( )
{
    return systimer -> clo;
}

void systimer_update ( uint32_t offset )
{
    // First, clear interrupt
    systimer -> cs = SYSTIMER_MATCH1;

    // Then, set next tick
    systimer -> c1 = ( systimer -> clo ) + offset;
}
