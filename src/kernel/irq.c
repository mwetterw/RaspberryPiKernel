#include "scheduler.h"
#include "bcm2835/pic.h"

void irq_dispatch ( void * oldSP )
{
    scheduler_handler ( oldSP );
}
