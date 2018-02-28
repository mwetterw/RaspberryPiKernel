#include "scheduler.h"
#include "bcm2835/pic.h"
#include "bcm2835/uart.h"

void * irq_dispatch ( void * oldSP )
{
    if ( pic_irq_pending ( IRQ_TIMER1 ) )
    {
        return scheduler_handler ( oldSP );
    }

    return oldSP;
}
