#include "semaphore.h"
#include "pcb_turnstile.h"

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
    uint8_t count;
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

sem_t sem_create ( uint8_t count )
{
    // Browse all semaphores until we find a free one
    for ( int i = 0 ; i < SEM_MAX ; ++i )
    {
        if ( sems [ i ].state == SEM_FREE )
        {
            sems [ i ].state = SEM_USED;
            sems [ i ].count = count;
            return i;
        }
    }

    return -1;
}

void sem_destroy ( sem_t sem )
{
    // Bound check
    if ( sem < 0 || sem >= SEM_MAX )
    {
        return;
    }

    // Semaphore is already free. No need to destroy...
    if ( sems [ sem ].state == SEM_FREE )
    {
        return;
    }

    // Let's free the semaphore
    // TODO Release waiting processes from the semaphore (if any)
    sems [ sem ].state = SEM_FREE;
    sems [ sem ].count = 0;
}
