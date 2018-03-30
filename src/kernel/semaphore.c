#include "semaphore.h"
#include "pcb_turnstile.h"
#include "scheduler.h"

#define SEM_MAX 32

enum
{
    SEM_FREE,
    SEM_USED,
};

struct semaphore
{
    kernel_pcb_turnstile_t waitqueue;
    int state;
    int count;
};

static struct semaphore sems [ SEM_MAX ];


void sem_init ( )
{
    for ( int i = 0 ; i < SEM_MAX ; ++i )
    {
        pcb_turnstile_init ( & ( sems [ i ].waitqueue ) );
        sems [ i ].state = SEM_FREE;
        sems [ i ].count = 0;
    }
}

sem_t sem_create ( int count )
{
    if ( count < 0 )
    {
        return -1;
    }

    uint32_t irqmask = irq_disable ( );

    // Browse all semaphores until we find a free one
    for ( int i = 0 ; i < SEM_MAX ; ++i )
    {
        if ( sems [ i ].state == SEM_FREE )
        {
            sems [ i ].state = SEM_USED;
            sems [ i ].count = count;
            irq_restore ( irqmask );
            return i;
        }
    }

    irq_restore ( irqmask );
    return -1;
}

void sem_destroy ( sem_t sem )
{
    // Bound check
    if ( sem < 0 || sem >= SEM_MAX )
    {
        return;
    }

    uint32_t irqmask = irq_disable ( );

    // Semaphore is already free. No need to destroy...
    if ( sems [ sem ].state == SEM_FREE )
    {
        return;
    }

    // Let's free the semaphore
    sems [ sem ].state = SEM_FREE;
    sems [ sem ].count = 0;

    // Release waiting processes from the semaphore (if any)
    kernel_pcb_turnstile_t * waitq = & ( sems [ sem ].waitqueue );
    while ( ! pcb_turnstile_empty ( waitq ) )
    {
        kernel_pcb_t * pcb = pcb_turnstile_popfront ( waitq );
        pcb_turnstile_pushback ( pcb, &turnstile_round_robin );
    }

    irq_restore ( irqmask );
}

int wait ( sem_t sem )
{
    // Bound check
    if ( sem < 0 || sem >= SEM_MAX )
    {
        return -1;
    }

    uint32_t irqmask = irq_disable ( );

    if ( sems [ sem ].state != SEM_USED )
    {
        irq_restore ( irqmask );
        return -1;
    }

    sems [ sem ].count--;

    if ( sems [ sem ].count < 0 )
    {
        pcb_turnstile_remove ( pcb_running, &turnstile_round_robin );
        pcb_turnstile_pushback ( pcb_running, & ( sems [ sem ].waitqueue ) );

        scheduler_yield ( );
    }

    irq_restore ( irqmask );

    return 0;
}

int signal ( sem_t sem )
{
    // Bound check
    if ( sem < 0 || sem >= SEM_MAX )
    {
        return -1;
    }

    uint32_t irqmask = irq_disable ( );

    if ( sems [ sem ].state != SEM_USED )
    {
        irq_restore ( irqmask );
        return -1;
    }

    sems [ sem ].count++;

    // Put the pcb from waiting to ready state
    if ( sems [ sem ].count <= 0 )
    {
        kernel_pcb_turnstile_t * waitq = & ( sems [ sem ].waitqueue );
        kernel_pcb_t * pcb = pcb_turnstile_popfront ( waitq );
        pcb_turnstile_pushback ( pcb, &turnstile_round_robin );
    }
    irq_restore ( irqmask );

    return 0;
}
